#include "TrackingRoutine.hpp"

#include <TimestampedFrame.hpp>
#include "settings/TrackingRoutineSettings.hpp"

#include <opencv2/highgui.hpp>

#include <QtGui/QImage>

#include <QtMath>

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
    qDebug() << Q_FUNC_INFO << "Destroying the object";
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
            m_currentTimestamp = frame.timestamp();
            if (!frame.image().isNull()) {
                doTracking(frame);
                emit trackedAgents(m_agents);
            }
            else
                qDebug() << Q_FUNC_INFO << "Input image does not exist";
        }
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
 * Puts an image to the debug queue if the corresponding flag is active.
 */
void TrackingRoutine::enqueueDebugImage(const cv::Mat& image)
{
    if (m_enqueueDebugFrames) {
//        cv::imshow("Background", image);
        // copy the image to be placed in the queue
        cv::Mat* debugImage = new cv::Mat(image.rows, image.cols, image.type());
        image.copyTo(*debugImage);

        // and push it to the queue
        std::chrono::milliseconds ms = std::chrono::milliseconds();
        TimestampedFrame frame(debugImage, ms);
        m_debugQueue->enqueue(frame);
    }
}

/*!
 * Assign detected objects to ids.
 */
void TrackingRoutine::assingIds(IdsAssignmentMethod method, std::vector<cv::Point2f>& centers, std::vector<float> directions)
{
    switch (method) {
    case IdsAssignmentMethod::NAIVE_CLOSEST_NEIGHBOUR:
        naiveClosestNeighbour(centers, directions);
        break;
    default:
        naiveClosestNeighbour(centers, directions);
        break;
    }
}

/*!
 * The ids assignment method originally used in CATS.
 * NOTE FIXME : this method is potentially erroneous in many cases, as the order of assigment is defined by the indeces of agents in the list.
 */
void TrackingRoutine::naiveClosestNeighbour(std::vector<cv::Point2f>& centers, std::vector<float> directions)
{
    std::vector<size_t> remainingAgents(m_agents.size());
    for (size_t i = 0; i < remainingAgents.size(); ++i)
        remainingAgents[i] = i;

    for (size_t i = 0; i < centers.size(); ++i) {
        float minDistance = std::numeric_limits<float>::max();
        size_t agentIndex = 0;
        bool detected = false;
        for (size_t j = 0; j < remainingAgents.size(); ++j) {
            const StateImage& agentState = m_agents[remainingAgents[j]].state();
            cv::Point2f const& blobCenter = centers[i];
            double const distance = qSqrt((blobCenter.x - agentState.position().x()) * (blobCenter.x - agentState.position().x()) +
                                          (blobCenter.y - agentState.position().y()) * (blobCenter.y - agentState.position().y()));
            if (distance < minDistance) {
                minDistance = distance;
                agentIndex = remainingAgents[j];
            }
            detected = true;
        }

        if (detected) {
            AgentDataImage& agent = m_agents[agentIndex];
            if (directions.size() > 0) {
                agent.mutableState()->setPosition(centers[i]);
                agent.mutableState()->setOrientation(directions[i]);
            }
            else {
                agent.mutableState()->setPosition(centers[i]);
                agent.mutableState()->invalidateOrientation();
            }

            agent.setTimestamp(m_currentTimestamp);
            // remove this agent's index from the list
            remainingAgents.erase(std::remove(remainingAgents.begin(), remainingAgents.end(), agentIndex), remainingAgents.end()); // https://en.wikipedia.org/wiki/Erase–remove_idiom
        }
    }

    // invalidate the state for agents whose positions were not updated
    for(size_t j = 0; j < remainingAgents.size(); ++j) {
        m_agents[remainingAgents[j]].mutableState()->invalidateState();
    }
}
