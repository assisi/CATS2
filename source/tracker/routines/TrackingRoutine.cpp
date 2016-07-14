#include "TrackingRoutine.hpp"

#include <QtGui/QImage>

/*!
* Constructor.
*/
TrackingRoutine::TrackingRoutine(TimestampedFrameQueuePtr inputQueue, TimestampedFrameQueuePtr debugQueue) :
    QObject(),
    _inputQueue(inputQueue),
    _debugQueue(debugQueue),
    _stopped(false),
    _enqueueDebugFrames(false)
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
    _stopped = false;
    TimestampedFrame frame;

    while (!_stopped) {
        if (_inputQueue.data()->dequeue(frame)) {
            doTracking(frame);
        }
        emit trackedAgents(_agents);
    }

    emit finished();
}

/*!
 * Stops the convertor.
 */
void TrackingRoutine::stop()
{
    _stopped = true;
}

/*!
 * Starts/stops to enqueue the debug images to the debug queue.
 */
void TrackingRoutine::sendDebugImages(bool send)
{
    if (_enqueueDebugFrames != send) {
        if (!_enqueueDebugFrames) // i.e. we are not sending at the moment
            _debugQueue.clear(); // empty the debug queue if by chance there are old frames

        _enqueueDebugFrames = send;
    }
}

