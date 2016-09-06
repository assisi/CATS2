#ifndef CATS2_TRACKING_ROUTINE_HPP
#define CATS2_TRACKING_ROUTINE_HPP

#include <CommonPointerTypes.hpp>
#include <AgentData.hpp>

#include <QtCore/QObject>

#include <opencv2/video.hpp>
#include <opencv2/video/background_segm.hpp>

#include <atomic>

class TimestampedFrame;

/*!
 * \brief The method of the ids' assignment.
 */
enum class IdsAssignmentMethod
{
    NAIVE_CLOSEST_NEIGHBOUR // the one originally used in CATS
    // TODO : add other methods
};


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

public:
    //! Returns a pointer to the debug queue.
    TimestampedFrameQueuePtr debugQueue() { return m_debugQueue; }
    //! Reports on what type of agent can be tracked by this routine.
    virtual QList<AgentType> capabilities() const = 0;

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
    void onSendDebugImages(bool send);

protected:
    //! The tracking routine excecuted. Gets the original frame, detects
    //! agents, eventually associates them with the trajectories and
    //! enqueue debug images on request.
    virtual void doTracking(const TimestampedFrame& frame) = 0;
    //! Puts an image to the debug queue.
    void enqueueDebugImage(const cv::Mat& image);
    //! Sets the states of all agents as invalid. The goal is to prevent the
    //! outdated data from poping; it's up to the specific routine to set
    //! the agent's state as valid if it is detected.
    void invalidateAgentsState();

protected:
    //! Assign detected objects to ids.
    void assingIds(IdsAssignmentMethod method, std::vector<cv::Point2f>& centers, std::vector<float> directions = std::vector<float>());
    //! The ids assignment method originally used in CATS.
    //! NOTE FIXME : this method is potentially erroneous in many cases, as the order of assigment is defined by the indeces of agents in the list.
    void naiveClosestNeighbour(std::vector<cv::Point2f>& centers, std::vector<float> directions = std::vector<float>());

protected:
    //! The queue containing frames to do the tracking.
    TimestampedFrameQueuePtr m_inputQueue;
    //! The debug frames queue to control the work of a tracker.
    TimestampedFrameQueuePtr m_debugQueue; // TODO : replaced this one queue by a list of queues to debug various phases of the tracking routine

    //! The current frame's timestamp.
    std::chrono::milliseconds m_currentTimestamp;
    //! The flag that defines if the convertor is to be stopped.
    std::atomic_bool m_stopped;
    //! The flag that defines if the debug images are to be put to the debug queue.
    std::atomic_bool m_enqueueDebugFrames;

    //! The tracked agents.
    QList<AgentDataImage> m_agents;
};

#endif // CATS2_TRACKING_ROUTINE_HPP
