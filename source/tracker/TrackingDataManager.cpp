#include "TrackingDataManager.hpp"

#include <QtCore/QDebug>

constexpr std::chrono::duration<double> TrackingDataManager::MaxTimeDifferenceMs;
constexpr double TrackingDataManager::IdentityDistanceThresholdMeters;

/*!
 * Constructor.
 */
TrackingDataManager::TrackingDataManager() :
    QObject(nullptr),
    m_primaryDataSource(SetupType::UNDEFINED),
    m_primaryDataSourceCapability(AgentType::UNDEFINED),
    m_typeForGenericAgents(AgentType::FISH) // TODO : find a better way to do this?
{

}

/*!
 * Adds new data source to the list. Also defines what kind of objects this
 * source is able to track.
 */
void TrackingDataManager::addNewDataSource(SetupType::Enum setupType,
                                           QList<AgentType> capabilities)
{
    // add new data source
    m_trackingData[setupType] = QQueue<TimestampedWorldAgentData>();

    // update the primary data source if necessary
    foreach (AgentType capability, capabilities) {
        if (capability < m_primaryDataSourceCapability) {
            m_primaryDataSourceCapability = capability;
            m_primaryDataSource = setupType;
        }
    }
}

/*!
 * New tracking results arrive.
 */
void TrackingDataManager::onNewData(SetupType::Enum setupType, TimestampedWorldAgentData timestampedAgentsData)
{
    // if the new data comes from a secondary data source then it is stored in the input queue
    if (setupType != m_primaryDataSource) {
        if (m_trackingData.contains(setupType)) {
            m_trackingData[setupType].enqueue(timestampedAgentsData);
        } else {
            qDebug() << Q_FUNC_INFO << "Unknown data source: " << SetupType::toString(setupType);
        }
    } else {
        // if the data comes from the primary source then it needs to be treated right away
        QList<AgentDataWorld> mergedAgentData = timestampedAgentsData.agentsData;
        // get the new data's timestamp
        std::chrono::milliseconds timestamp = timestampedAgentsData.timestamp;
        // look throught data from other sources to make a merge
        bool sendData = true;
        foreach (SetupType::Enum dataSource, m_trackingData.keys()) {
            if (dataSource != m_primaryDataSource) {
                // we look for the data with the closest timestamp.
                TimestampedWorldAgentData closestAgentData;
                if (getDataByTimestamp(timestamp, m_trackingData[dataSource], closestAgentData)) {
                    // if the data list is found than we take the agent from this list that are not
                    // yet in the final list
                    QList<AgentDataWorld> newAgentData;
                    foreach (AgentDataWorld agentData, closestAgentData.agentsData) {
                        if (! findAgentInList(agentData, mergedAgentData)) {
                            newAgentData.append(agentData);
                        }
                    }
                    mergedAgentData.append(newAgentData);
                } else {
                    sendData = false; // i.e. we consider that all sources should contribute
                    // TODO : this parameter should be a flat in the configuration file
                }
            }
        }

        if (sendData) {
            // set the type of all the agent of the undefined type to the specified type
            for (size_t i = 0; i < mergedAgentData.size(); i++) {
                if (mergedAgentData[i].type() == AgentType::GENERIC)
                    mergedAgentData[i].setType(m_typeForGenericAgents);
            }

            // send the results
            notifyAgentDataMerged(mergedAgentData);
        }
    }
}

/*!
 * Find the best match to the provided timestamp in the given queue.
 */
bool TrackingDataManager::getDataByTimestamp(std::chrono::milliseconds timestamp,
                                             QQueue<TimestampedWorldAgentData>& dataQueue,
                                             TimestampedWorldAgentData& bestMatchData)
{
    if (dataQueue.size() == 0) {
//        qDebug() << Q_FUNC_INFO << "The data queue is empty";
        return false;
    }

    // if the data in the queue is more recent than the timestamp
    // (this can only happen if by some mistery we get the data from
    // the deep past)
    if (dataQueue.head().timestamp >= timestamp) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(dataQueue.head().timestamp - timestamp) < MaxTimeDifferenceMs) { // FIXME : no need in cast timestamp is already the duration value
            bestMatchData = dataQueue.dequeue();
            return true;
        } else {
            qDebug() << Q_FUNC_INFO << "Data with an old timestamped received";
            return false;
        }
    }

    // a normal situation when the timestamp to find is newer than the queue's head
    bool dataFound = false;
    std::chrono::duration<double> minTimeDifferenceMs = MaxTimeDifferenceMs;
    while ((dataQueue.size() > 0) && (!dataFound)) {
        // the data is too old
        if (std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - dataQueue.head().timestamp) > MaxTimeDifferenceMs) {
            dataQueue.dequeue(); // forget old data
        } else { // if we are still in the old data
            if (timestamp > dataQueue.head().timestamp) { // then we cache the last used data (*)
                bestMatchData = dataQueue.dequeue();
                minTimeDifferenceMs = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - bestMatchData.timestamp);
            } else { // we happen to pass to the new data now
                if (std::chrono::duration_cast<std::chrono::milliseconds>(dataQueue.head().timestamp - timestamp) <  minTimeDifferenceMs) {
                    bestMatchData = dataQueue.dequeue();
                }
                dataFound = true;
            }
        }
    }

    // manage the case (*) when we might take all the data from the queue while approaching the
    // timestamp that is too new
    if ((!dataFound) && (dataQueue.size()==0) && (minTimeDifferenceMs < MaxTimeDifferenceMs))
        dataFound = true;

    return dataFound;
}

/*!
 * Checks if the agent's position and orientation correspond to another agent
 * in the list.
 */
bool TrackingDataManager::findAgentInList(AgentDataWorld agentToFind, QList<AgentDataWorld> agentDataList)
{
    // we need to compare the positions and orientations when available wiht
    // the thresholds that would be hardcoded at the moment but that will need
    // to be set in the configuration file and throught the GUI
    bool agentFound = false;
    foreach (AgentDataWorld agent, agentDataList) {
        if (agent.state().position().distanceTo(agentToFind.state().position()) < IdentityDistanceThresholdMeters) {
            agentFound = true;
            break;
            // TODO : to add the orientation comparison
        }
    }

    return agentFound;
}
