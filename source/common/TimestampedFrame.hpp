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
    explicit TimestampedFrameQueue(int initialSize = DefaultInitialSize, size_t maxSize = DefaultMaxSize):
        m_queue(initialSize),
        m_maxSize(maxSize)
    { }
    //! Destructor.
    virtual ~TimestampedFrameQueue() final { }

public:
    //! The queue became too big.
    bool isOutgrown() const { return (m_queue.size_approx() > m_maxSize); }
    //! Drops one element from the tail.
    void dropTail()
    {
        TimestampedFrame frameToForget;
        m_queue.try_dequeue(frameToForget);
    }

    //! Adds an element to the queue.
    void enqueue(const TimestampedFrame& frame)
    {
        if (!m_queue.try_enqueue(frame))
            qDebug() << Q_FUNC_INFO << "Could not enqueue new element, skipping";

//        qDebug() <<  Q_FUNC_INFO << QString("Frames queue contains %1 elements").arg(_queue.size_approx());
    }

    //! Gets an element from the queue, returns true if succeded.
    bool dequeue(TimestampedFrame& frame)
    {
        bool result = m_queue.try_dequeue(frame);
        // if nothing in the queue then wait a bit
        if (!result) {
//            qDebug() << Q_FUNC_INFO << "Failed to get an element from the queue, is it empty?";
            std::this_thread::sleep_for( std::chrono::milliseconds(TimeOutMs));
        }
//        } else
//            qDebug() << Q_FUNC_INFO << "Took one element from the queue";

        return result;
    }

private:
    //! The queue to store data.
    moodycamel::ReaderWriterQueue<TimestampedFrame> m_queue;
    //! The max number of elements that can be put to the queue. Since it seems to be
    //! not supported by the "moodycamel::ReaderWriterQueue" to limit the size of the queue
    //! we need to store it separately.
    size_t m_maxSize;

    //! Dequeueing time out.
    static const int TimeOutMs;  // [ms]
    //! Default maximal queue size.
    static constexpr size_t DefaultMaxSize = 1500;
    //! Default initial queue size.
    static constexpr size_t DefaultInitialSize = 300;
};

#endif // CATS2_TIMESTAMPED_FRAME_HPP

