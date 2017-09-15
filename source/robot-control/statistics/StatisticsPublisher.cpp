#include "StatisticsPublisher.hpp"

#include <settings/RobotControlSettings.hpp>

#include <zmqHelpers.hpp>

#include <QtCore/QDebug>
#include <QtCore/QThread>

constexpr char StatisticsPublisher::SubscriberAddress[];
constexpr char StatisticsPublisher::PublisherAddress[];

/*!
 * The singleton getter. Provides an instance of the settings.
 */
StatisticsPublisher& StatisticsPublisher::get()
{
    static StatisticsPublisher instance;
    return instance;
}

/*!
 * Constructor.
 */
StatisticsPublisher::StatisticsPublisher() :
    QObject(nullptr),
    m_context(),
    m_publisher(m_context, ZMQ_PUB)
{
    try {
        m_publisher.bind(PublisherAddress);
        qDebug() << QString("Sender binds %1").arg(PublisherAddress);
    } catch (const zmq::error_t& e) {
        qDebug() <<  QString("Exception while connecting to %1").arg(PublisherAddress)
                  << e.what();
    }

    // start the timer to update statistics
    // start the control timer
    int stepMsec = 300;
    connect(&m_updateTimer, &QTimer::timeout, [=](){ publishStatistics(); });
    m_updateTimer.start(stepMsec);

    // launch the tracking routine in a separated thread
    QStringList addresses;
    addresses.append(SubscriberAddress);
    m_subscriber = StaticsticsSubscriberPtr(new StaticsticsSubscriber(m_context, addresses));
    QThread* thread = new QThread;
    m_subscriber->moveToThread(thread);
    connect(thread, &QThread::started, m_subscriber.data(), &StaticsticsSubscriber::process);
    connect(m_subscriber.data(), &StaticsticsSubscriber::finished, thread, &QThread::quit);
    // FIXME : for some reasons the slot is not triggered without Qt::DirectConnection
    connect(m_subscriber.data(), &StaticsticsSubscriber::getStatisticsReceived,
            this, &StatisticsPublisher::onGetStatisticsReceived, Qt::DirectConnection);
    connect(m_subscriber.data(), &StaticsticsSubscriber::postStatisticsReceived,
            this, &StatisticsPublisher::onPostStatisticsReceived, Qt::DirectConnection);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

/*!
 * Destructor.
 */
StatisticsPublisher::~StatisticsPublisher()
{
    qDebug() << "Destroying the object";
}

/*!
 * Registers new statistics.
 */
void StatisticsPublisher::addStatistics(QString id)
{
    if (!m_statistics.contains(id)) {
        m_statistics[id] = 0;
        qDebug() << QString("Registered statistics %1").arg(id);
    } else {
        qDebug() << QString("Attempting to register statistics %1 multiple "
                            "times").arg(id);
    }
}

/*!
 * Updates the statistics value.
 */
void StatisticsPublisher::updateStatistics(QString id, double value)
{
    if (m_statistics.contains(id))
        m_statistics[id] = value;
    else
        qDebug() << QString("Attempting to update an unregistered statistics %1")
                    .arg(id);
}

/*!
 * Posts the statistics.
 */
void StatisticsPublisher::publishStatistics()
{
    QString message;
    for (auto& id : m_statisticsToPost) {
        message.append(id);
        message.append(":");
        message.append(QString::number(m_statistics[id], 'f', 3));
        message.append(";");
    }
    // remove last ";"
    if (message.size() > 0)
        message = message.left(message.length() - 1);

    // sends the data
    std::string data = message.toStdString();
    std::string name = "optimiser";
    std::string device = "";
    std::string command = "update";
    if (data.size() > 0)
        sendMessage(name, device, command, data);
}

/*!
 * Publishes the message.
 */
void StatisticsPublisher::sendMessage(std::string& name, std::string& device,
                                    std::string& command, std::string& data)
{
    try {
        zmq::sendMultipart(m_publisher, name, device, command, data);
        qDebug() << "Message sent"
                 << QString::fromStdString(name)
                 << QString::fromStdString(device)
                 << QString::fromStdString(command)
                 << QString::fromStdString(data);
    } catch (const zmq::error_t& e) {
        qDebug() <<  QString("Exception while sending messages")
                  << e.what();
    }
}

/*!
 * Processes input get statistics messages. Sends the list of all available
 * statistics.
 */
void StatisticsPublisher::onGetStatisticsReceived()
{
    std::string data = m_statistics.keys().join(";").toStdString();
    std::string name = "optimiser";
    std::string device = "";
    std::string command = "statistics";

    sendMessage(name, device, command, data);
}

/*!
 * Processes input post statistics requests. Updates the list of statistics to
 * publish.
 */
void StatisticsPublisher::onPostStatisticsReceived(QStringList statisticsIdsList)
{
    m_statisticsToPost.clear();
    for (auto& id : statisticsIdsList) {
        if (m_statistics.contains(id))
            m_statisticsToPost.append(id);
    }
}
