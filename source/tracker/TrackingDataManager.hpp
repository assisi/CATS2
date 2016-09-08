#ifndef CATS2_TRACKING_DATA_MANAGER_HPP
#define CATS2_TRACKING_DATA_MANAGER_HPP

#include <SetupType.hpp>
#include <AgentData.hpp>

#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QtMath>

#include <chrono>

/*!
 * \brief The class that receives the tracking data from different trackers, analyses it to find the
 * common data, merges together, sends out to the viewer and writes to the disk.
 * NOTE : this class shold be managed with the smart pointers instead of the Qt's parenting mechanism.
 */
class TrackingDataManager : public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit TrackingDataManager();

    //! Adds new data source to the list. Also defines what kind of objects this source
    //! is able to track.
    void addNewDataSource(SetupType::Enum setupType, QList<AgentType> capabilities);

signals:
    //! The results of merging the data from various sources.
    void notifyAgentDataMerged(QList<AgentDataWorld> agentsData);

public slots:
    //! New tracking results arrive.
    void onNewData(SetupType::Enum setupType, TimestampedWorldAgentData agentsData);

private:
    //! Find the best match to the provided timestamp in the given queue.
    bool getDataByTimestamp(std::chrono::milliseconds timestamp,
                              QQueue<TimestampedWorldAgentData>&,
                              TimestampedWorldAgentData&);
    //! Merges together two data sets by adding newly received agents to
    //! already available and removing the duplicates.
    void matchAgents(QList<AgentDataWorld>& currentAgents, QList<AgentDataWorld>& agentsToJoin, QList<AgentDataWorld>& joinedAgentsList);
    //! Computes the distances and angles between the agents while analyzing this values to remove the impossible combinations.
    float initializeCostMatrices(const QList<AgentDataWorld>& listOne, const QList<AgentDataWorld>& listTwo, QVector<QVector<float>>& costMatrix);
    //! Generates all the possible ordered combinations k indices (shorter list one) out of n indices (longer list two),
    //! taking into account which combinations make sense.
    void searchBestMatch(int depth, int k, int n, QVector<bool>& usedIndices,
                         QVector<int>& combination, QVector<int>& bestCombination,
                         float& minCost, const QVector<QVector<float>>& costMatrix);
    //! Computes the costs of the given combination.
    float getCombinationCost(const QVector<int>& combination,
                                                 const QVector<QVector<float>>& costMatrix);

private:
    //! The tracking results recieved from various sources.
    QMap<SetupType::Enum, QQueue<TimestampedWorldAgentData>> m_trackingData;

    //! The source whose input that triggers the input data processing,
    //! normally it's the source that provide the most advanced data.
    SetupType::Enum m_primaryDataSource;
    //! The corresponding capabilities, i.e. the type of agent the primary
    //! source provides.
    AgentType m_primaryDataSourceCapability;

    //! This agent type is set by default for all agents with undefined type.
    //! For instance we might consider all agents of the undefined type as fishes.
    AgentType m_typeForGenericAgents;

    //! Max acceptable time difference to search for the corresponding timestamps.
    static constexpr std::chrono::duration<double> MaxTimeDifferenceMs = std::chrono::milliseconds(100);

    // TODO : to move this values out to the settings
    //! If two agents are closer than this value they are considered as the same.
    static constexpr float IdentityDistanceThresholdMeters = 0.025; // [m], i.e. 2.5 cm
    //! The threshold for the orientations to check that they correspond.
    static constexpr float OrientationThresholdRad = M_PI / 8; // i.e. 22.5 degrees
    //! The weight coefficient for the angle in the cost function.
    static constexpr float OrientationWeightCoefficient = 0.001 * 180 / M_PI; // i.e. 10 degrees of disallignment correspond to the offset of 1 cm
    //! The weighted threshold taking into account the orientation and distance thresholds.
    static constexpr float WeightedThreshold = IdentityDistanceThresholdMeters + OrientationThresholdRad * OrientationWeightCoefficient;
    //! The penalty for the invalid orientation.
    static constexpr float InvalidOrientationPenaltyRad = M_PI / 16; // i.e. 11.25 degrees
};

#endif // CATS2_TRACKING_DATA_MANAGER_HPP
