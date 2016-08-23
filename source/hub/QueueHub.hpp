#ifndef CATS2_QUEUE_HUB_HPP
#define CATS2_QUEUE_HUB_HPP

#include "HubPointerTypes.hpp"

#include <CommonPointerTypes.hpp>

/*!
 * \brief The class that provides one input queue and several output queues. The data from the input queue
 * is copied to the output queues.
 */
class QueueHub
{
public:
    //! Constructor.
    explicit QueueHub(TimestampedFrameQueuePtr inputQueue);
    //! Destructor.
    virtual ~QueueHub();

public:
    //! Creates new queue, add it to the dispatcher output list and returns the pointer.
    TimestampedFrameQueuePtr addOutputQueue();

private:
    //! The tracking routine that tracks agents on the scene.
    //! Doesn't have a Qt owner as it is managed by another thread.
    MultiplicatorPtr m_multiplicator;
};

#endif // CATS2_QUEUE_HUB_HPP
