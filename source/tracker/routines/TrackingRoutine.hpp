#ifndef CATS2_TRACKING_ROUTINE_HPP
#define CATS2_TRACKING_ROUTINE_HPP

#include <CommonPointerTypes.hpp>
#include <AgentData.hpp>

#include <QtCore/QObject>

#include <atomic>

class TimestampedFrame;

/*!
* \brief Parent class for various tracking routines.
*/
class TrackingRoutine : public QObject
{
    Q_OBJECT
public:
    //! Constructor. Gets the input queue to process and a queue to place debug images on request.
    explicit TrackingRoutine(TimestampedFrameQueuePtr inputQueue, TimestampedFrameQueuePtr debugQueue);
    //! Destructor.
    virtual ~TrackingRoutine();

signals:
    //! Sends out the tracked agents.
    void trackedAgents(QList<AgentDataImage> agents);
    //! Notifies that the tracking is stopped.
    void finished();
    //! Notifies about an error.
    void error(QString errorMessage);

public slots:
    //! Starts the tracking.
    void process();
    //! Stops the tracking.
    void stop();
    //! Starts/stops to enqueue the debug images to the debug queue.
    void sendDebugImages(bool send);

protected:
    //! The tracking routine excecuted. Gets the original frame, detects
    //! agents, eventually associates them with the trajectories and
    //! enqueue debug images on request.
    virtual void doTracking(const TimestampedFrame& frame) = 0;

protected:
    //! The queue containing frames to do the tracking.
    TimestampedFrameQueuePtr m_inputQueue;
    //! The debug frames queue to control the work of a tracker.
    TimestampedFrameQueuePtr m_debugQueue;

    //! The flag that defines if the convertor is to be stopped.
    std::atomic_bool m_stopped;
    //! The flag that defines if the debug images are to be put to the debug queue.
    std::atomic_bool m_enqueueDebugFrames;

    //! The tracked agents.
    QList<AgentDataImage> m_agents;
};

/*!
 * The alias for the pointer to the tracking routine class.
 */
using TrackingRoutinePtr = QSharedPointer<TrackingRoutine>;


#endif // CATS2_TRACKING_ROUTINE_HPP
