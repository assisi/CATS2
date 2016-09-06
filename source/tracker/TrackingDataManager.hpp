#ifndef CATS2_TRACKING_DATA_MANAGER_HPP
#define CATS2_TRACKING_DATA_MANAGER_HPP

#include <SetupType.hpp>
#include <AgentData.hpp>

#include <QtCore/QObject>
#include <QtCore/QQueue>

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
    //! Checks if the agent's position and orientation correspond to another agent in the list.
    bool findAgentInList(AgentDataWorld agentData, QList<AgentDataWorld> agentDataList);

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

    //! If two agents are closer than this value they are considered as the same.
    static constexpr double IdentityDistanceThresholdMeters = 0.005; // [m], i.e. 5 mm
};

#endif // CATS2_TRACKING_DATA_MANAGER_HPP
