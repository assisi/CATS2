#include "Subscriber.hpp"
#include "zmqHelpers.hpp"

#include <QtCore/QDebug>

#include <thread>

/*!
 * Constructor. Creates the subscriber socket on the provided address.
*/
Subscriber::Subscriber(zmq::context_t& context,
                       QStringList subscriberAddresses):
    m_subscriber(context, ZMQ_SUB)
{
    // we accept all messages
    m_subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    // connect to the socket
    for (QString address : subscriberAddresses) {
        try {
            m_subscriber.connect(address.toStdString().c_str());
            qDebug() << QString("Subscriber is connected to %1").arg(address);
        } catch (const zmq::error_t& e) {
            qDebug() <<  QString("Exception while connecting to %1").arg(address)
                      << e.what();
        }
    }
}

/*!
* Destructor.
*/
Subscriber::~Subscriber()
{
    qDebug() << "Destroying the object";
}

/*!
 * Starts listening.
 */
void Subscriber::process()
{
    m_stopped = false;

    std::string name;
    std::string device;
    std::string command;
    std::string data;

    try {
        // receive and recode incoming messages
        while (!m_stopped) {
            if (m_subscriber.connected() && recvMultipart(m_subscriber, name, device, command, data)) {
                qDebug() << "Message received"
                         << QString::fromStdString(name)
                         << QString::fromStdString(device) << QString::fromStdString(command) << QString::fromStdString(data);
                // TODO : analyse the type of the message and to send the
                // corresponding message
                processMessage(name, device, command, data);
            } else {
                // if no data available when we make a pause
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    } catch (const zmq::error_t& e) {
        qDebug() <<  QString("Exception while receiving messages")
                  << e.what();
    }
    emit finished();
}

/*!
 * Stops listening.
 */
void Subscriber::stop()
{
    m_stopped = true;
}

/*!
 * Processes the input message.
 */
void Subscriber::processMessage(std::string name, std::string device,
                                std::string command, std::string data)
{
    // at the moment we manage the binary-choice message only
    if (QString(command.data()).toLower().contains("casu")) {
        if (QString(device.data()).toLower().contains("message")) {
            m_beeDensities[QString(command.data()).toLower()] =
                    QString(data.data()).toDouble();

            // by agreement when we get the data from the second casu we decide
            // on the turning direction
            if (QString(command.data()).toLower().contains("002")) {
                QString message;
                if (m_beeDensities["casu-001"] < m_beeDensities["casu-002"])
                    message = "CW";
                else
                    message = "CCW";
                emit notifyBeeSetCircularSetupTurningDirection(message);
            }
        }
    }

}
