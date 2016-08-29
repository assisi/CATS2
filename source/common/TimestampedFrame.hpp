#ifndef CATS2_TIMESTAMPED_FRAME_HPP
#define CATS2_TIMESTAMPED_FRAME_HPP

#include <readerwriterqueue.h>

#include <opencv2/core/core.hpp>

#include <QtCore/QDebug>
#include <QtCore/QSharedPointer>

#include <chrono>
#include <thread>

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
    virtual ~TimestampedFrame() final;

public:
    //! Returns the frame image.
    QSharedPointer<cv::Mat> image() const { return m_image; }
    //! Returns the time stamp.
    std::chrono::milliseconds timestamp() const { return m_timestamp; }

private:
    //! The frame image.
    QSharedPointer<cv::Mat> m_image;
    //! The corresponding timestamp, in number of milliseconds since 1970-01-01T00:00:00
    //! Universal Coordinated Time.
    std::chrono::milliseconds m_timestamp;
};

/*!
 * The queue of the timestamped frames.
 */
class TimestampedFrameQueue
{
public:
    //! Constructor.
    explicit TimestampedFrameQueue(int initialSize = DefaultInitialSize, size_t maxSize = DefaultMaxSize, bool dropTailWhenOutgrown = true):
        m_queue(initialSize),
        m_maxSize(maxSize),
        m_dropTailWhenOutgrown(dropTailWhenOutgrown)
    { }
    //! Destructor.
    virtual ~TimestampedFrameQueue() final { }

public:
    //! The queue became too big.
    bool isOutgrown() const { return (m_queue.size_approx() > m_maxSize); }

    //! Drops one element from the head.
    void dropHead()
    {
        TimestampedFrame frameToForget;
        m_queue.try_dequeue(frameToForget);
    }

    //! Empties the queue.
    void empty()
    {
        TimestampedFrame frame;
        int cnt = 0;
        while (m_queue.try_dequeue(frame))
            cnt++;
        qDebug() << Q_FUNC_INFO << QObject::tr("Dropped %1 elements").arg(cnt);
    }

    //! Adds an element to the queue.
    void enqueue(const TimestampedFrame& frame)
    {
        // first check if there is allows place in the queue otherwise remove the tail element
        if (isOutgrown() && m_dropTailWhenOutgrown) {
            dropHead();
        }

        // enqueue the new element
        if (!m_queue.try_enqueue(frame))
            qDebug() << Q_FUNC_INFO << "Could not enqueue new element, skipping";
    }

    //! Gets an element from the queue, returns true if succeded.
    bool dequeue(TimestampedFrame& frame)
    {
        bool result = m_queue.wait_dequeue_timed(frame, std::chrono::milliseconds(TimeOutMs));
        return result;
    }

private:
    //! The queue to store data.
    moodycamel::BlockingReaderWriterQueue<TimestampedFrame> m_queue;
    //! The max number of elements that can be put to the queue. Since it seems to be
    //! not supported by the "moodycamel::ReaderWriterQueue" to limit the size of the queue
    //! we need to store it separately.
    size_t m_maxSize;
    //! Defines if the queue should drop the tail element when trying to add new element and
    //! the size is too big
    bool m_dropTailWhenOutgrown;

    //! Dequeueing time out.
    static const int TimeOutMs;  // [ms]
    //! Default maximal queue size.
    static constexpr size_t DefaultMaxSize = 1500;
    //! Default initial queue size.
    static constexpr size_t DefaultInitialSize = 300;
};

#endif // CATS2_TIMESTAMPED_FRAME_HPP

