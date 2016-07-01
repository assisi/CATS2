#include "StreamReceiver.hpp"
#include <QGst/Parse>
#include <QGlib/Connect>
#include <QGst/Bus>
#include <QGst/Message>
#include <QGlib/Error>

/*!
 * The map to translate the string stream type to the corresponding enum.
 */
const QMap<QString, StreamType> StreamDescriptor::_streamTypeByName = {{"v4l", StreamType::VIDEO_4_LINUX}};

/*!
* Constructor for a typified input stream.
*/
StreamReceiver::StreamReceiver(StreamDescriptor streamParameters, TimestampedFrameQueuePtr outputQueue) :
    QObject(),
    _pipe1ineDescription(),
    _sink(outputQueue)
{
    switch (streamParameters.streamType()) {
    case StreamType::VIDEO_4_LINUX:
    {
        int videoDeviceId = streamParameters.parameters().toInt();
        _pipe1ineDescription = QString("v4l2src device=\"/dev/video%1\" ! "
                                       "decodebin ! "
                                       "appsink name=\"queueingsink\" ").arg(videoDeviceId);
        break;
    }
    case StreamType::UNDEFINED:
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
    _pipeline = QGst::Parse::launch(_pipe1ineDescription).dynamicCast<QGst::Pipeline>();
    _sink.setElement(_pipeline->getElementByName("queueingsink"));
    QGlib::connect(_pipeline->bus(), "message::error", this, &StreamReceiver::onError);

    // start the pipeline
    _pipeline->setState(QGst::StatePlaying);
}

/*!
 * Stops the receiver.
 */
void StreamReceiver::stop()
{
    _pipeline->setState(QGst::StateNull);
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



