#include "TrackingDataManager.hpp"
#include "settings/TrackingSettings.hpp"

#include <CoordinatesConversion.hpp>

constexpr std::chrono::duration<double> TrackingDataManager::MaxTimeDifferenceMs;
constexpr float TrackingDataManager::IdentityDistanceThresholdMeters;
constexpr float TrackingDataManager::OrientationThresholdRad;
constexpr float TrackingDataManager::OrientationWeightCoefficient;
constexpr float TrackingDataManager::WeightedThreshold;
constexpr float TrackingDataManager::InvalidOrientationPenaltyRad;

/*!
 * Constructor.
 */
TrackingDataManager::TrackingDataManager() :
    QObject(nullptr),
    m_primaryDataSource(SetupType::UNDEFINED),
    m_primaryDataSourceCapability(AgentType::UNDEFINED),
    m_typeForGenericAgents(AgentType::FISH), // TODO : find a better way to do this(?)
    m_trajectoryWriter()
{

}

/*!
 * Destructor.
 */
TrackingDataManager::~TrackingDataManager()
{

}

/*!
 * Adds new data source to the list. Also defines what kind of objects this
 * source is able to track.
 */
void TrackingDataManager::addDataSource(SetupType::Enum setupType,
                                           QList<AgentType> capabilities)
{
    // add new data source
    m_trackingData[setupType] = QQueue<TimestampedWorldAgentData>();

    // update the primary data source if necessary (smaller value means more data)
    foreach (AgentType capability, capabilities) {
        if (capability < m_primaryDataSourceCapability) {
            m_primaryDataSourceCapability = capability;
            m_primaryDataSource = setupType;
        }
    }
}

/*!
 * Adds new coordinates conversion.
 */
void TrackingDataManager::addCoordinatesConversion(SetupType::Enum setupType, CoordinatesConversionPtr coordinatesConversion)
{
    m_coordinatesConversions.insert(setupType, coordinatesConversion);
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
        QList<AgentDataWorld> agentDataList = timestampedAgentsData.agentsData;
        // get the new data's timestamp
        std::chrono::milliseconds timestamp = timestampedAgentsData.timestamp;
        // the flag that defines if all data sources could be merged together
        bool allDataMerged = true;
        // look throught data from other sources to make a merge
        foreach (SetupType::Enum dataSource, m_trackingData.keys()) {
            if (dataSource != m_primaryDataSource) {
                // we look for the data with the closest timestamp.
                TimestampedWorldAgentData closestAgentData;
                if (getDataByTimestamp(timestamp, m_trackingData[dataSource], closestAgentData)) {
                    // if the data list is found than we take the agent from this list that are not
                    // yet in the final list
                    QList<AgentDataWorld> mergedAgentDataList;
                    matchAgents(agentDataList, closestAgentData.agentsData, mergedAgentDataList);
                    // update the agent data list with the newly merged data
                    agentDataList = mergedAgentDataList;
                } else {
                    allDataMerged = false;
                }
            }
        }

        // set the type of all the agent of the undefined type to the specified type
        for (auto& agentData : agentDataList) {
            if (agentData.type() == AgentType::GENERIC)
                agentData.setType(m_typeForGenericAgents);
        }

        // send the results
        emit notifyAgentDataWorldMerged(agentDataList);

        // sends results in main setup image coordinates
        // NOTE : this is a temporary code to share results with CATS
        QList<AgentDataImage> agentDataListImage = convertToFrameCoordinates(SetupType::MAIN_CAMERA, agentDataList);
        emit notifyAgentDataImageMerged(agentDataListImage);

        // save the results to a file only if all the data could be merged
        if (allDataMerged)
            m_trajectoryWriter.writeData(timestamp, agentDataList);
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
 * Merges together two data sets by adding newly received agents to already
 * available and removing the duplicates.
 */
void TrackingDataManager::matchAgents(QList<AgentDataWorld>& currentAgents, QList<AgentDataWorld>& agentsToJoin, QList<AgentDataWorld>& joinedAgentsList)
{
    // set the aliases
    QList<AgentDataWorld>& listOne(currentAgents.size() < agentsToJoin.size() ? currentAgents : agentsToJoin);
    QList<AgentDataWorld>& listTwo(currentAgents.size() < agentsToJoin.size() ? agentsToJoin : currentAgents);
    // result : list one is never longer than list two

    // cost matrices
    QVector<QVector<float>> costMatrix(listOne.size());
    float maxCost = initializeCostMatrices(listOne, listTwo, costMatrix);

    // TODO : uncomment to debug agent matching
    qDebug() << Q_FUNC_INFO;
    qDebug() << "Cost matrix" ;
    qDebug() << "Threshold" << WeightedThreshold;
    qDebug() << "Max cost" << maxCost;
    for (int i = 0; i < listOne.size(); i++)
        qDebug() << costMatrix[i];

    // find best match
    QVector<bool> usedIndices(listTwo.size(), false);
    QVector<int> combination(listOne.size(), -1);
    QVector<int> bestCombination(listOne.size(), -1);
    float minCost = maxCost + 1; // +1 to make it work for single agent case when minCost is equal to maxCost
    searchBestMatch(0, listOne.size(), listTwo.size(), usedIndices, combination, bestCombination,  minCost, costMatrix);

    // generate the joined list
    // first add duplicated agents to the output list
    QList<QPair<int, int>> indecesToRemove;
    for (int i1 = 0; i1 < listOne.size(); i1++) {
        // analyse the winning combination to remove elements that are too far
        if (costMatrix[i1][bestCombination[i1]] < WeightedThreshold) { // i.e. the elements are close enough
            const AgentDataWorld& agentOne = listOne.at(i1);
            const AgentDataWorld& agentTwo = listTwo.at(bestCombination[i1]);
            // keep the agent with the smaller type (i.e. containing more information)
            if (agentOne.type() < agentTwo.type())
                joinedAgentsList.append(agentOne);
            else
                joinedAgentsList.append(agentTwo);
            indecesToRemove.append(qMakePair(i1, bestCombination[i1]));
        } else {
//            qDebug() <<  Q_FUNC_INFO << costMatrix[i1][bestCombination[i1]] << WeightedThreshold;
        }
    }
    // remove duplicated elements from both input lists
    for (int i = 0; i < indecesToRemove.size(); i++) {
        // remove these agents from both lists
        listOne.removeAt(indecesToRemove[i].first);
        listTwo.removeAt(indecesToRemove[i].second);
    }

    // add remaining items
    joinedAgentsList.append(listOne);
    joinedAgentsList.append(listTwo);
}

/*!
 * Computes the distances and angles between the agents while analyzing this
 * values to remove the impossible combinations.
 */
float TrackingDataManager::initializeCostMatrices(const QList<AgentDataWorld>& listOne,
                                                 const QList<AgentDataWorld>& listTwo,
                                                 QVector<QVector<float>>& costMatrix)
{
    // set sizes
    for (int i = 0; i < listOne.size(); i++)
        costMatrix[i].fill(0, listTwo.size());

    float angle;
    float distance;
    float maxCost = 0;
    // fill the costs matrices
    for (int i1 = 0; i1 < listOne.size(); i1++){
        for (int i2 = 0; i2 < listTwo.size(); i2++) {
            // compute the distance
            distance = listOne[i1].state().position().distanceTo(listTwo[i2].state().position());

            // compute the minimal angle between two agents
            if (listOne[i1].state().orientation().isValid() && listTwo[i2].state().orientation().isValid()) {
                angle = listOne[i1].state().orientation().angle() - listTwo[i2].state().orientation().angle();
                // normalize to [-pi;pi]
                if (angle < -M_PI)
                    angle += M_PI;
                // map to [-pi/2;pi/2]
                if (angle > M_PI_2)
                    angle -= M_PI;
                if (angle < -M_PI_2)
                    angle += M_PI;

                angle = qAbs(angle);
            } else
                angle = InvalidOrientationPenaltyRad;

            costMatrix[i1][i2] = distance + OrientationWeightCoefficient * angle;
            // increas the max cost value
            maxCost += costMatrix[i1][i2];
        }
    }

    return maxCost;
}

/*!
 * Generates all the possible ordered combinations k indices (shorter list one)
 * out of n indices (longer list two), taking into account which combinations
 * make sense.
 */
void TrackingDataManager::searchBestMatch(int depth,  // depth of the recursion = current elemetn in the combination
                                           int k, int n, // we compute A{^k}{_n}
                                           QVector<bool>& usedIndices,  // tells which values are already used in the combination
                                           QVector<int>& combination,  // the combination that is constructed
                                           QVector<int>& bestCombination,  // the combination that is constructed
                                           float& minCost,
                                           const QVector<QVector<float>>& costMatrix)
{
    if (depth == k) {
        float cost = getCombinationCost(combination, costMatrix);
        if (cost < minCost) {
            bestCombination = combination;
            minCost = cost;
        }
        return;
    }

    for (int i = 0; i < n; i++) {
        if (! usedIndices[i]) {
            usedIndices[i] = true;
            combination[depth] = i;
            searchBestMatch(depth + 1, k, n, usedIndices, combination, bestCombination, minCost, costMatrix);
            usedIndices[i] = false;
        }
    }
}

/*!
 * Computes the costs of the given combination.
 */
float TrackingDataManager::getCombinationCost(const QVector<int>& combination,
                                             const QVector<QVector<float>>& costMatrix)
{
    float cost = 0;
    for (int i1 = 0; i1 < combination.size(); i1++)
        cost += costMatrix[i1][combination[i1]];
    return cost;
}

/*!
 * Converts a list of agent data objects from world to image coordinates.
 */
QList<AgentDataImage> TrackingDataManager::convertToFrameCoordinates(SetupType::Enum setupType, QList<AgentDataWorld> mergedAgentDataList)
{
    QList<AgentDataImage> agentsDataImageList;
    // if the conversion is available
    if (m_coordinatesConversions.contains(setupType)) {
        // convert all agents
        foreach (AgentDataWorld agentDataWorld, mergedAgentDataList) {
            PositionPixels imagePosition = m_coordinatesConversions[setupType]->worldToImagePosition(agentDataWorld.state().position());
            OrientationRad imageOrientation = m_coordinatesConversions[setupType]->worldToImageOrientationRad(agentDataWorld.state().position(),
                                                                                                              agentDataWorld.state().orientation());
            agentsDataImageList.append(AgentDataImage(agentDataWorld.id(), agentDataWorld.type(), StateImage(imagePosition, imageOrientation)));
        }
    }
    return agentsDataImageList;
}
