#ifndef CATS2_INTERSPECIES_DATA_MANAGER_HPP
#define CATS2_INTERSPECIES_DATA_MANAGER_HPP

#include "Subscriber.hpp"

#include <msg/AgentData.pb.h>
#include <AgentData.hpp>

#include <zmq.hpp>

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>


/*!
 * Gets messages via qt signals and sends them through zmq.
 * TODO : to add the reception part
 * NOTE : data classes should be managed through smart pointers without using
 * the Qt's mechanism of ownership; thus we set the parent to nullptr in the
 * constructor.
 */
class InterSpeciesDataManager : public QObject
{
    Q_OBJECT
public:
    //! Constructor. Creates the publisher socket on the provided address.
    explicit InterSpeciesDataManager(QString publisherAddress,
                                     QStringList subscriberAddresses);

signals:
    //! Notifies on the turning direction deduced from the bee setup bees (CW/CCW).
    void notifyBeesSetCircularSetupTurningDirection(QString message);
    //! Notifies that the robot must update its target position
    void notifyRobotTargetPositionUpdated(QString message);
    //! Notifies the reception of a command to change robot mode in 2Rooms Inter-species experiments
    void notifyInterspecies2RoomsModeChange(QString message);

public slots:
    //! Triggered when new agent data is to be published.
    //! NOTE: seems like obsolete, it was done to sent data to CATS originally.
    void publishAgentData(QList<AgentDataImage> agentDataList);
    //! Triggered when new data on the fish group and robot rotation direction
    //! arrive from the circular experiment.
    void publishCircularExperimentData(QString agentId,
                                       QString fishTurningDirection,
                                       QString robotTurningDirection);

    //! Triggered when statistics about the circular experiment are to be published.
    void publishCircularExperimentStatistics(QString agentId,
                                             double fishClockWisePercent,
                                             double fishCounterClockWisePercent,
                                             QString fishTurningDirection,
                                             double robotClockWisePercent,
                                             double robotCounterClockWisePercent,
                                             QString robotTurningDirection);

    //! Triggered when robot target position is changed
    void publishRobotTargetPosition(QString agentId,
                                    PositionMeters position);

private:
    //! Publishes the message.
    //! name : message type, used to restore the message.
    //! device : message data producer (if applied).
    //! desc : supplementary data // TODO : to check
    //! data : message data.
    void publishMessage(std::string& name,std::string& device,std::string& desc,std::string& data);

    //! Converts the agent type to the string.
    std::string agentTypeToString(AgentType agentType);

private:
    //! The zmq context, one should use exactly one context in a process.
    zmq::context_t m_context;
    //! The publisher socket.
    zmq::socket_t m_publisher;
    //! The subscriber receiving the data from the remote system.
    SubscriberPtr m_subscriber;
};

using InterSpeciesDataManagerPtr = QSharedPointer<InterSpeciesDataManager>;

#endif // CATS2_INTERSPECIES_DATA_MANAGER_HPP
