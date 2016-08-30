#include "Multiplicator.hpp"

#include <TimestampedFrame.hpp>

#include <QtCore/QMutexLocker>

/*!
* Constructor.
*/
Multiplicator::Multiplicator(TimestampedFrameQueuePtr inputQueue) :
    QObject(),
    m_inputQueue(inputQueue),
    m_stopped(false)
{
}

/*!
* Destructor.
*/
Multiplicator::~Multiplicator()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

/*!
 * Starts the dispatcher. It reads the frames from the input queue, and duplicates the frames to several output queues.
 * TODO : consider if it is possible to not duplicate the data by copying only the pointers. The risk is that several threads
 * might try to modify the same image thus corrupting it, and thus some kind of a protection mechanism is needed. One option would be
 * to modify the TimestampedFrame::image() getter to return a copy of the image instead of the pointer, but it means that we would still
 * need to copy the data. Other option is to return a const reference to the image, but this could end with the dangling reference when the
 * TimestampedFrame's object is deleted. And, by the way, OpenCV gives no possibility to describe a const reference to an image, since cv::Mat
 * is a shared pointer.
 * http://stackoverflow.com/questions/13713625/is-cvmat-class-flawed-by-design
 */
void Multiplicator::process()
{
    if (!m_inputQueue.isNull()) {
        m_stopped = false;
        // read the frames
        while (!m_stopped) {
            TimestampedFrame frame;
            if (m_inputQueue->dequeue(frame)) {
                m_outputQueuesMutex.lock();
                // all output queues but the very first
                for (size_t i = m_outputQueues.size() - 1; i > 0; --i) {
                    TimestampedFrame newFrame(frame);
                    m_outputQueues[i]->enqueue(newFrame);
                }
                // and the very first queue gets the original image
                m_outputQueues[0]->enqueue(frame);
                m_outputQueuesMutex.unlock();
            }
        }
    } else {
        qDebug() << Q_FUNC_INFO << "Input queue is not set, finishing.";
    }

    emit finished();
}

/*!
 * Stops the dispatcher.
 */
void Multiplicator::stop()
{
    m_stopped = true;
}

/*!
 * Adds new output queue to the list.
 */
void Multiplicator::addOutputQueue(TimestampedFrameQueuePtr newQueue)
{
    QMutexLocker locker(&m_outputQueuesMutex);
    m_outputQueues.append(newQueue);
}
