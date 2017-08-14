#include "SettingsInterface.hpp"

#include <settings/RobotControlSettings.hpp>

#include <zmqHelpers.hpp>

#include <QtCore/QDebug>
#include <QtCore/QThread>

constexpr char SettingsInterface::ReceiverAddress[];
constexpr char SettingsInterface::SenderAddress[];

SettingsInterface::SettingsInterface() :
    QObject(nullptr),
    m_context(),
    m_sender(m_context, ZMQ_PUB)
{
    try {
        m_sender.bind(SenderAddress);
        qDebug() << QString("Sender binds %1").arg(SenderAddress);
    } catch (const zmq::error_t& e) {
        qDebug() <<  QString("Exception while connecting to %1").arg(SenderAddress)
                  << e.what();
    }

    // launch the tracking routine in a separated thread
    QStringList addresses;
    addresses.append(ReceiverAddress);
    m_receiver = ReceiverPtr(new Receiver(m_context, addresses));
    QThread* thread = new QThread;
    m_receiver->moveToThread(thread);
    connect(thread, &QThread::started, m_receiver.data(), &Receiver::process);
    connect(m_receiver.data(), &Receiver::finished, thread, &QThread::quit);
    // FIXME : for some reasons the slot is not triggered without Qt::DirectConnection
    connect(m_receiver.data(), &Receiver::getRequestReceived,
            this, &SettingsInterface::onGetRequestReceived, Qt::DirectConnection);
    connect(m_receiver.data(), &Receiver::setRequestReceived,
            this, &SettingsInterface::onSetRequestReceived, Qt::DirectConnection);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

/*!
 * Destructor.
 */
SettingsInterface::~SettingsInterface()
{
    qDebug() << "Destroying the object";
}

/*!
 * Publishes the message.
 */
void SettingsInterface::sendMessage(std::string& name, std::string& device,
                                    std::string& command, std::string& data)
{
    try {
        zmq::sendMultipart(m_sender, name, device, command, data);
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
 * Processes input set messages. Sets the value in the settings corresponding
 * to the path provided.
 */
void SettingsInterface::onSetRequestReceived(std::string path, double value)
{
    QStringList pathList = QString::fromStdString(path).split("/");
    if (pathList.size() > 0) {
        // NOTE : at the moment only robot's settings are exposed
        if (pathList[0] == "robots") {
            RobotControlSettings::get().setValueByPath(path, value);
        }
    }
}

/*!
 * Processes input get messages. Finds the value in the settings that
 * corresponds to the path in the configuration path given by the request
 * string.
 */
void SettingsInterface::onGetRequestReceived(std::string path)
{
    QStringList pathList = QString::fromStdString(path).split("/");
    if (pathList.size() > 0) {
        // NOTE : at the moment only robot's settings are exposed
        if (pathList[0] == "robots") {
            double value = RobotControlSettings::get().valueByPath(path);

            // FIXME : using std::to_string(value) put comma instead of dot in
            // decimals
            std::string data = QString::number(value).toStdString();//std::to_string(value);
            std::string name = "optimiser";
            std::string device = path;
            std::string command = "set";
            sendMessage(name, device, command, data);
        }
    }
}
