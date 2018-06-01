#include "InterSpeciesDataManager.hpp"

#include "zmqHelpers.hpp"

#include <AgentData.hpp>
#include <MessageTypes.hpp>

#include <QtCore/QDebug>
#include <QtCore/QThread>

InterSpeciesDataManager::InterSpeciesDataManager(QString publisherAddress,
                                                 QStringList subscriberAddresses) :
    QObject(nullptr),
    m_context(1),
    m_publisher(m_context, ZMQ_PUB)
{
    try {
        //m_publisher.connect(publisherAddress.toStdString().data());
        m_publisher.bind(publisherAddress.toStdString().data()); // XXX does not work with connect -- use "bind" instead !
        qDebug() << QString("Publisher is connected to %1").arg(publisherAddress);
    } catch (const zmq::error_t& e) {
        qDebug() <<  QString("Exception while connecting to %1").arg(publisherAddress)
                  << e.what();
    }

    // launch the tracking routine in a separated thread
    m_subscriber = SubscriberPtr(new Subscriber(m_context, subscriberAddresses));
    QThread* thread = new QThread;
    m_subscriber->moveToThread(thread);
    connect(thread, &QThread::started, m_subscriber.data(), &Subscriber::process);
    connect(m_subscriber.data(), &Subscriber::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(m_subscriber.data(), &Subscriber::notifyBeeSetCircularSetupTurningDirection,
            this, &InterSpeciesDataManager::notifyBeesSetCircularSetupTurningDirection);
    connect(m_subscriber.data(), &Subscriber::notifyReceptionOfUpdateRobotTargetPositionMessage,
            this, &InterSpeciesDataManager::notifyRobotTargetPositionUpdated);
    thread->start();
}

/*!
 * Publishes the message.
 */
void InterSpeciesDataManager::publishMessage(std::string& name,std::string& device,std::string& desc,std::string& data)
{
    qDebug() << "Sending message:"
             << QString::fromStdString(name)
             << QString::fromStdString(device)
             << QString::fromStdString(desc)
             << QString::fromStdString(data);
    try {
        zmq::sendMultipart(m_publisher, name, device, desc, data);
    } catch (const zmq::error_t& e) {
        qDebug() <<  QString("Exception while sending messages")
                  << e.what();
    }
}

/*!
 * Triggered when new agent data is to be published.
 */
void InterSpeciesDataManager::publishAgentData(QList<AgentDataImage> agentDataList)
{
    // convert the data to a message
    CatsMessages::AgentDataList agentDataToPublish;

    foreach (AgentDataImage agentData, agentDataList) {
        CatsMessages::AgentData* agentDataMessage = agentDataToPublish.add_agentsdata();
        agentDataMessage->set_id(agentData.id().toStdString());
        agentDataMessage->set_type(agentTypeToString(agentData.type()));
        agentDataMessage->mutable_state()->set_angle(agentData.state().orientation().angleRad());
        agentDataMessage->mutable_state()->set_x(agentData.state().position().x());
        agentDataMessage->mutable_state()->set_y(agentData.state().position().y());

        //qDebug() << agentData.id() << agentData.state().position().toString();
    }

    // send the data
    std::string name = std::to_string(MessageTypes::AGENT_DATA); // FIXME : and if the order of messages changes?
    std::string device = "";
    std::string desc = "";
    std::string data;
    agentDataToPublish.SerializeToString(&data);
    publishMessage(name, device, desc, data);
}

/*!
 * Converts the agent type to the string.
 */
std::string InterSpeciesDataManager::agentTypeToString(AgentType agentType)
{
    switch (agentType) {
    case AgentType::CASU:
        return "fish_casu";
        break;
    case AgentType::FISH:
        return "fish";
        break;
    case AgentType::GENERIC:
        return "generic_agent";
        break;
    case AgentType::UNDEFINED:
    default:
        return "undefined";
    }
}

/*!
 * Triggered when new data on the fish group and robot rotation direction
 * arrive from the circular experiment.
 */
void InterSpeciesDataManager::publishCircularExperimentData(QString agentId,
                                                            QString fishTurningDirection,
                                                            QString robotTurningDirection)
{
    std::string message;
    message.append("fish:");
    message.append(fishTurningDirection.toStdString());
    message.append(",");
    message.append("fishCasu:");
    message.append(robotTurningDirection.toStdString());

    std::string name = "casu-001";
    std::string device = "CommEth";
    std::string command = "cats";
    publishMessage(name, device, command, message);

    name = "casu-002";
    publishMessage(name, device, command, message);
}


void InterSpeciesDataManager::publishCircularExperimentStatistics(QString agentId,
                                                                  double fishClockWisePercent,
                                                                  double fishCounterClockWisePercent,
                                                                  QString fishTurningDirection,
                                                                  double robotClockWisePercent,
                                                                  double robotCounterClockWisePercent,
                                                                  QString robotTurningDirection)
{
    std::string message;
    message.append("fishClockWisePercent:");
    message.append(QString::number(fishClockWisePercent).toStdString());
    message.append(";");
    message.append("fishCounterClockWisePercent:");
    message.append(QString::number(fishCounterClockWisePercent).toStdString());
    message.append(";");
    message.append("fishTurningDirection:");
    //message.append(TurningDirection::toString(fishTurningDirection).toStdString());
    message.append(fishTurningDirection.toStdString());
    message.append(";");
    message.append("robotClockWisePercent:");
    message.append(QString::number(robotClockWisePercent).toStdString());
    message.append(";");
    message.append("robotCounterClockWisePercent:");
    message.append(QString::number(robotCounterClockWisePercent).toStdString());
    message.append(";");
    message.append("robotTurningDirection:");
    //message.append(TurningDirection::toString(robotTurningDirection).toStdString());
    message.append(robotTurningDirection.toStdString());
    message.append(";");

    std::string name = "";
    std::string messageType = "Statistics";
    std::string sender = "setup-1"; // TODO
    publishMessage(name, messageType, sender, message);
}


void InterSpeciesDataManager::publishRobotTargetPosition(QString agentId,
                                                         PositionMeters position)
{
    std::string message;
    message.append("x:");
    message.append(QString::number(position.x()).toStdString());
    message.append(";");
    message.append("y:");
    message.append(QString::number(position.y()).toStdString());
    message.append(";");

    std::string name = "";
    std::string messageType = "RobotTargetPositionChanged";
    std::string sender = agentId.toStdString(); // TODO
    publishMessage(name, messageType, sender, message);
}

