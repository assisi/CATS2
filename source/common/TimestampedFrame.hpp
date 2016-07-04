#ifndef CATS2_TIMESTAMPED_FRAME_HPP
#define CATS2_TIMESTAMPED_FRAME_HPP

#include <readerwriterqueue.h>

#include "opencv2/core/core.hpp"

#include <QtCore/QDebug>
#include <QtCore/QSharedPointer>

#include <chrono>

/*!
* \brief This class stores a video frame image and its timestamp
*
*/
class TimestampedFrame
{
public:
    //! Constructor.
    explicit TimestampedFrame(cv::Mat* image = nullptr, std::chrono::milliseconds timestamp = std::chrono::milliseconds());
    //! Destructor.
    virtual ~TimestampedFrame();

public:
    // get setters and getters here

private:
    //! The frame image.
    QSharedPointer<cv::Mat> _image;
    //! The corresponding timestamp, in number of milliseconds since 1970-01-01T00:00:00
    //! Universal Coordinated Time.
    std::chrono::milliseconds _timestamp;
};

/*!
 * The queue of the timestamped frames.
 */
class TimestampedFrameQueue
{
public:
    //! Constructor.
    explicit TimestampedFrameQueue(int initialSize = DefaultInitialSize, size_t maxSize = DefaultMaxSize):
        _queue(initialSize),
        _maxSize(maxSize)
    { }
    //! Destructor.
    virtual ~TimestampedFrameQueue() { }

public:
    //! The queue became too big.
    bool isOutgrown() { return (_queue.size_approx() > _maxSize); }
    //! Drops one element from the tail.
    void dropTail()
    {
        TimestampedFrame frameToForget;
        _queue.try_dequeue(frameToForget);
    }

    void enqueue(TimestampedFrame frame)
    {
        if (!_queue.try_enqueue(frame))
            qDebug() << "Could not enqueue new element";

        qDebug() << QString("Input queue contains %1 elements").arg(_queue.size_approx());
    }

private:
    //! The queue to store data.
    moodycamel::ReaderWriterQueue<TimestampedFrame> _queue;
    //! The max number of elements that can be put to the queue. Since it seems to be
    //! not supported by the "moodycamel::ReaderWriterQueue" to limit the size of the queue
    //! we need to store it separately.
    size_t _maxSize;

    //! Default maximal queue size.
    static constexpr size_t DefaultMaxSize = 1500;
    //! Default initial queue size.
    static constexpr size_t DefaultInitialSize = 300;
};

/*!
 * The alias for the shared pointer to a queue of the timestamped frames.
 */
using TimestampedFrameQueuePtr = QSharedPointer<TimestampedFrameQueue>;

#endif // CATS2_TIMESTAMPED_FRAME_HPP

