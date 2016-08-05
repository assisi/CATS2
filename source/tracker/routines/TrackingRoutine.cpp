#include "TrackingRoutine.hpp"

#include <TimestampedFrame.hpp>
#include "settings/TrackingRoutineSettings.hpp"

#include <QtGui/QImage>

/*!
* Constructor.
*/
TrackingRoutine::TrackingRoutine(TimestampedFrameQueuePtr inputQueue, TimestampedFrameQueuePtr debugQueue) :
    QObject(),
    m_inputQueue(inputQueue),
    m_debugQueue(debugQueue),
    m_stopped(false),
    m_enqueueDebugFrames(false)
{
}

/*!
* Destructor.
*/
TrackingRoutine::~TrackingRoutine()
{

}

/*!
 * Starts the convertor.
 */
void TrackingRoutine::process()
{
    m_stopped = false;
    TimestampedFrame frame;

    while (!m_stopped) {
        if (m_inputQueue->dequeue(frame)) {
            doTracking(frame);
        }
        emit trackedAgents(m_agents);
    }

    emit finished();
}

/*!
 * Stops the convertor.
 */
void TrackingRoutine::stop()
{
    m_stopped = true;
}

/*!
 * Starts/stops to enqueue the debug images to the debug queue.
 */
void TrackingRoutine::onSendDebugImages(bool send)
{
    if (m_enqueueDebugFrames != send) {
        if (!m_enqueueDebugFrames) // i.e. we are not sending at the moment
            m_debugQueue->empty(); // empty the debug queue if by chance there are old frames

        m_enqueueDebugFrames = send;
    }
}

/*!
 * Puts an image to the debug queue.
 */
void TrackingRoutine::enqueueDebugImage(const cv::Mat& image)
{
    // copy the image to be placed in the queue
    cv::Mat* debugImage = new cv::Mat(image.rows, image.cols, CV_8UC3);
    image.copyTo(*debugImage);

    // and push it to the queue
    std::chrono::milliseconds ms = std::chrono::milliseconds();
    TimestampedFrame frame(debugImage, ms);
    m_debugQueue->enqueue(frame);
}
