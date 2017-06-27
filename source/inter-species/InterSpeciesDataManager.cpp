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
    m_publisher.bind(publisherAddress.toStdString().data());

    // launch the tracking routine in a separated thread
    m_subscriber = SubscriberPtr(new Subscriber(m_context, subscriberAddresses));
    QThread* thread = new QThread;
    m_subscriber->moveToThread(thread);
    connect(thread, &QThread::started, m_subscriber.data(), &Subscriber::process);
    connect(m_subscriber.data(), &Subscriber::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

/*!
 * Publishes the message.
 */
void InterSpeciesDataManager::publishMessage(std::string& name,std::string& device,std::string& desc,std::string& data)
{
    zmq::sendMultipart(m_publisher, name, device, desc, data);
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
