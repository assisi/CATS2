#include "StreamReceiver.hpp"

#include <QGst/Parse>
#include <QGlib/Connect>
#include <QGst/Bus>
#include <QGst/Message>
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
    m_sink(outputQueue, targetFrameSize)
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
                                            "ffdec_h264 ! ffmpegcolorspace ! "
                                            "deinterlace ! "
                                            "video/x-raw-rgb ! ffmpegcolorspace !"
                                            "appsink name=queueingsink").arg(streamParameters.parameters());
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

    m_pipeline = QGst::Parse::launch(m_pipelineDescription).dynamicCast<QGst::Pipeline>();
    m_sink.setElement(m_pipeline->getElementByName("queueingsink"));
    QGlib::connect(m_pipeline->bus(), "message::error", this, &StreamReceiver::onError);

    // start the pipeline
    m_pipeline->setState(QGst::StatePlaying);
}

/*!
 * Stops the receiver.
 */
void StreamReceiver::stop()
{
    m_pipeline->setState(QGst::StateNull);
}

/*!
 * Called when an error is detected in the input pipelile. At the moment the behavior is very basic:
 * in case of any error the pipeline is stopped and never restarted.
 */
void StreamReceiver::onError(const QGst::MessagePtr & message)
{
    QString errorMessage;
    switch (message->type()) {
        case QGst::MessageEos:
            errorMessage = "End of stream ";
            break;
        case QGst::MessageError:
            errorMessage = message.staticCast<QGst::ErrorMessage>()->error().message();
            break;
        default:
            break;
    }
    qCritical() << errorMessage;
    stop();
    emit error(errorMessage);
}



