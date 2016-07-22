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
void TrackingRoutine::sendDebugImages(bool send)
{
    if (m_enqueueDebugFrames != send) {
        if (!m_enqueueDebugFrames) // i.e. we are not sending at the moment
            m_debugQueue.clear(); // empty the debug queue if by chance there are old frames

        m_enqueueDebugFrames = send;
    }
}

