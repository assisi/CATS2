#ifndef CATS2_MULTIPLICATOR_HPP
#define CATS2_MULTIPLICATOR_HPP

#include <CommonPointerTypes.hpp>

#include <QtCore/QObject>
#include <QtCore/QMutex>

#include <atomic>

/*!
 * Gets frames from the input queue and copies them to the output queue.
 */
class Multiplicator : public QObject
{
    Q_OBJECT
public:
    //! Constructor. Gets the input queue.
    explicit Multiplicator(TimestampedFrameQueuePtr inputQueue);
    //! Destructor.
    virtual ~Multiplicator();

    //! Adds new output queue to the list.
    void addOutputQueue(TimestampedFrameQueuePtr newQueue);

signals:
    //! Notifies that the dispatcher is stopped.
    void finished();

public slots:
    //! Starts the dispatcher.
    void process();
    //! Stops the dispatcher.
    void stop();

private:
    //! The queue containing frames to distribute.
    TimestampedFrameQueuePtr m_inputQueue;
    //! The list of output queues.
    QList<TimestampedFrameQueuePtr> m_outputQueues;
    //! A mutex protecting the list of output queues.
    QMutex m_outputQueuesMutex;

    //! The flag that defines if the convertor is to be stopped.
    std::atomic_bool m_stopped;
};

#endif // CATS2_MULTIPLICATOR_HPP
