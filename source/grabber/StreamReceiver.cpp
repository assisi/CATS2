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
 * The map to translate the string stream type to the corresponding enum.
 */
const QMap<QString, StreamType> StreamDescriptor::m_streamTypeByName = {{"v4l", StreamType::VIDEO_4_LINUX},
                                                                        {"vf", StreamType::LOCAL_VIDEO_FILE},
                                                                        {"if", StreamType::LOCAL_IMAGE_FILE}};

/*!
* Constructor.
*/
StreamReceiver::StreamReceiver(StreamDescriptor streamParameters, QSize targetFrameSize, TimestampedFrameQueuePtr outputQueue) :
    QObject(nullptr),
    m_pipelineDescription(),
    m_sink(outputQueue, targetFrameSize),
    m_restartOnEos(false)
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
            qDebug() << Q_FUNC_INFO << "Stream type is undefined";
        default:
            break;
    }
}

/*!
* Destructor.
*/
StreamReceiver::~StreamReceiver()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
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
        qDebug() << Q_FUNC_INFO << "Launching pipeline" << m_pipelineDescription;
        m_pipeline = QGst::Parse::launch(m_pipelineDescription).dynamicCast<QGst::Pipeline>();
        m_sink.setElement(m_pipeline->getElementByName("queueingsink"));
        QGlib::connect(m_pipeline->bus(), "message", this, &StreamReceiver::onMessage);

        m_pipeline->bus()->addSignalWatch();
        // start the pipeline
        m_pipeline->setState(QGst::StatePlaying);
    }
    catch (...) {
        qDebug() << Q_FUNC_INFO << "Exception while launching the pipeline, stopped";
        return;
    }
}

/*!
 * Stops the receiver.
 */
void StreamReceiver::stop()
{
    m_pipeline->setState(QGst::StateNull);
}

/*!
 * Tries to restart the video stream.
 */
void StreamReceiver::restart()
{
    QGst::SeekEventPtr event = QGst::SeekEvent::create(1.0, QGst::FormatTime,
                                                       QGst::SeekFlagFlush,
                                                       QGst::SeekTypeSet, 0.,
                                                       QGst::SeekTypeNone,
                                                       QGst::ClockTime::None);
    m_pipeline->sendEvent(event);
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



