#include "QueueHub.hpp"

#include"Multiplicator.hpp"

#include <TimestampedFrame.hpp>

#include <QtCore/QThread>

/*!
 * Constructor.
 */
QueueHub::QueueHub(TimestampedFrameQueuePtr inputQueue)
{
    // launch the incoming frames conversion in separated thread
    QThread* thread = new QThread;
    m_multiplicator = QSharedPointer<Multiplicator>(new Multiplicator(inputQueue),
                                                    &Multiplicator::deleteLater); // delete later is used for security as multithreaded
                                                                                  // signals and slots might result is crashes when a
                                                                                  // a sender is deleted before a signal is received for instance
    m_multiplicator->moveToThread(thread);

    QObject::connect(thread, &QThread::started, m_multiplicator.data(), &Multiplicator::process);
    QObject::connect(m_multiplicator.data(), &Multiplicator::finished, thread, &QThread::quit);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}

/*!
 * Destructor.
 */
QueueHub::~QueueHub()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
    m_multiplicator->stop();
}

/*!
 * Creates new queue, add it to the dispatcher output list and returns the pointer.
 */
TimestampedFrameQueuePtr QueueHub::addOutputQueue()
{
    // create new queue
    TimestampedFrameQueuePtr queue(new TimestampedFrameQueue(100));
    // add to the dispatcher's list
    m_multiplicator->addOutputQueue(queue);
    // return a pointer
    return queue;
}
