#include "StreamReceiver.hpp"

#include <QGst/Parse>
#include <QGlib/Connect>
#include <QGst/Bus>
#include <QGst/Message>
#include <QGst/Event>
#include <QGlib/Error>

#include <QtCore/QFileInfo>
#include <QtCore/QSize>

/*!
* Constructor.
*/
StreamReceiver::StreamReceiver(StreamDescriptor streamParameters, QSize expectedFrameSize, TimestampedFrameQueuePtr outputQueue) :
    QObject(nullptr),
    m_pipelineDescription(),
    m_sink(outputQueue, expectedFrameSize),
    m_restartOnEos(false),
    m_expectedFrameSize(expectedFrameSize)
{
    switch (streamParameters.streamType()) {
        case StreamType::VIDEO_4_LINUX:
        {
            int videoDeviceId = streamParameters.parameters().toInt();
            m_pipelineDescription = QString("v4l2src device=/dev/video%1 ! "
                                           "video/x-raw-rgb ! ffmpegcolorspace ! "
                                           "appsink name=queueingsink").arg(videoDeviceId);
            break;
        }
        case StreamType::LOCAL_VIDEO_FILE:
        {
            m_pipelineDescription = QString("filesrc location=%1 ! qtdemux ! "
                                            "decodebin ! ffmpegcolorspace ! "
                                            "deinterlace ! "
                                            "video/x-raw-rgb ! ffmpegcolorspace !"
                                            "appsink name=queueingsink").arg(streamParameters.parameters());
            m_restartOnEos = true;
            break;
        }
        case StreamType::LOCAL_IMAGE_FILE:
        {
            QFileInfo fileInfo(streamParameters.parameters());
            if (fileInfo.exists()) {
                QString decoder;
                if (fileInfo.suffix() == "png")
                    decoder = "pngdec";
                else if ((fileInfo.suffix() == "jpeg") || (fileInfo.suffix() == "jpg"))
                    decoder = "jpegdec";
                m_pipelineDescription = QString("filesrc location=%1 ! %2 ! "
                                                "ffmpegcolorspace ! videoscale ! imagefreeze !"
                                                "appsink name=queueingsink").arg(streamParameters.parameters()).arg(decoder);
            }
            break;
        }
        case StreamType::UNDEFINED:
            qDebug() << "Stream type is undefined";
        default:
            break;
    }
}

/*!
* Destructor.
*/
StreamReceiver::~StreamReceiver()
{
    qDebug() << "Destroying the object";
    stop();
}

/*!
 * Starts the receiver.
 */
void StreamReceiver::process()
{
    // check if the pipeline was set
    if (m_pipelineDescription.isEmpty())
        return;

    try {
        qDebug() << "Launching pipeline" << m_pipelineDescription;
        m_pipeline = QGst::Parse::launch(m_pipelineDescription).dynamicCast<QGst::Pipeline>();
        m_sink.setElement(m_pipeline->getElementByName("queueingsink"));
        QGlib::connect(m_pipeline->bus(), "message", this, &StreamReceiver::onMessage);

        // set the specific resolution on the video
        QString capsString = QString("video/x-raw-rgb, width=%1, height=%2")
                .arg(m_expectedFrameSize.width())
                .arg(m_expectedFrameSize.height());
        m_pipeline->getElementByName("queueingsink")->setProperty("caps", QGst::Caps::fromString(capsString));

        m_pipeline->bus()->addSignalWatch();
        // start the pipeline
        m_pipeline->setState(QGst::StatePlaying);
    }
    catch (const QGlib::Error & error) {
        qCritical() << "Failed to launch the pipeline:" << error;
        return;
    }
    catch (...) {
        qDebug() << "Exception while launching the pipeline, stopped";
        return;
    }
}

/*!
 * Stops the receiver.
 */
void StreamReceiver::stop()
{
    qDebug() << "Stopping the pipeline";
    try {
        m_pipeline->setState(QGst::StateNull);
    }
    catch (const QGlib::Error & error) {
        qCritical() << "Failed to stop the pipeline:" << error;
        return;
    }
    catch (...) {
        qDebug() << "Exception while stopping the pipeline";
        return;
    }
}

/*!
 * Tries to restart the video stream.
 */
void StreamReceiver::restart()
{
    qDebug() << "Restarting the pipeline";
    try {
        QGst::SeekEventPtr event = QGst::SeekEvent::create(1.0, QGst::FormatTime,
                                                           QGst::SeekFlagFlush,
                                                           QGst::SeekTypeSet, 0.,
                                                           QGst::SeekTypeNone,
                                                           QGst::ClockTime::None);
        m_pipeline->sendEvent(event);
    }
    catch (const QGlib::Error & error) {
        qCritical() << "Failed to restart the pipeline:" << error;
        return;
    }
    catch (...) {
        qDebug() << "Exception while restarting the pipeline";
        return;
    }
}

/*!
 * Called when an error is detected in the input pipelile. At the moment the behavior is very basic:
 * in case of any error the pipeline is stopped and never restarted.
 */
void StreamReceiver::onMessage(const QGst::MessagePtr & message)
{
    QString informationMessage;
    switch (message->type()) {
        case QGst::MessageEos:
            informationMessage = "End of stream ";
            if (m_restartOnEos)
                restart();
            break;
        case QGst::MessageError:
            informationMessage = message.staticCast<QGst::ErrorMessage>()->error().message();
            qCritical() << informationMessage;
            stop();
            emit error(informationMessage);
            break;
        default:
            break;
    }
}



