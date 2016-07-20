#include "VideoGrabber.hpp"

#include <QtCore/QThread>

/*!
* Constructor.
*/
VideoGrabber::VideoGrabber() : QObject(nullptr)
{

}

/*!
* Destructor.
*/
VideoGrabber::~VideoGrabber()
{
    stopAll();
}

/*!
 * Creates and launches a stream receiver in a separated thread.
 */
void VideoGrabber::addStreamReceiver(StreamDescriptor parameters, TimestampedFrameQueuePtr outputQueue)
{
    QThread* thread = new QThread;
    QSharedPointer<StreamReceiver> receiver = QSharedPointer<StreamReceiver>(new StreamReceiver(parameters, outputQueue), &QObject::deleteLater);
    // keep the receiver pointer
    _streamReceivers.append(receiver);

    receiver->moveToThread(thread);
    connect(receiver.data(), &StreamReceiver::error, this, &VideoGrabber::onError);
    connect(thread, &QThread::started, receiver.data(), &StreamReceiver::process);
    connect(receiver.data(), &StreamReceiver::destroyed, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

/*!
 * Deletes failed stream receiver.
 */
void VideoGrabber::onError(QString errorMessage)
{
    // find who sent the message
    StreamReceiver* streamReceiver = qobject_cast<StreamReceiver*>(QObject::sender());
    foreach (StreamReceiverPtr ptr , _streamReceivers) {
        if (ptr.data() == streamReceiver){
            // and remove it from the list, it will encure the deletion
            // of the receiver and of the corresponding thread
            _streamReceivers.removeAll(ptr);
            break;
        }
    }
}

/*!
 * Stops all the pipelines.
 */
void VideoGrabber::stopAll()
{
    foreach (StreamReceiverPtr ptr , _streamReceivers) {
        ptr->stop();
    }
}

