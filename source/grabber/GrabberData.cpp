#include "GrabberData.hpp"

#include "StreamReceiver.hpp"

#include <QtCore/QThread>
#include <QtCore/QSize>

/*!
* Constructor.
*/
GrabberData::GrabberData(StreamDescriptor parameters, QSize targetFrameSize, TimestampedFrameQueuePtr outputQueue) :
    QObject(nullptr)
{
    QThread* thread = new QThread;
    m_streamReceiver = StreamReceiverPtr(new StreamReceiver(parameters, targetFrameSize, outputQueue));

    m_streamReceiver->moveToThread(thread);
    connect(m_streamReceiver.data(), &StreamReceiver::error, this, &GrabberData::onError);
    connect(thread, &QThread::started, m_streamReceiver.data(), &StreamReceiver::process);
    connect(m_streamReceiver.data(), &StreamReceiver::destroyed, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

/*!
* Destructor.
*/
GrabberData::~GrabberData()
{
    qDebug() << "Destroying the object";
    if (!m_streamReceiver.isNull())
        m_streamReceiver->stop();
}

/*!
 * Deletes failed stream receiver.
 */
void GrabberData::onError(QString errorMessage)
{
    // delete the receiver and the corresponding thread
    m_streamReceiver.reset();
}


