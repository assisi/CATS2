#include "Subscriber.hpp"
#include "zmqHelpers.hpp"

#include <QtCore/QDebug>

#include <thread>

/*!
 * Constructor. Creates the subscriber socket on the provided address.
*/
Subscriber::Subscriber(zmq::context_t& context, QString subscriberAddress):
    m_subscriber(context, ZMQ_SUB)
{
    // we accept all messages
    m_subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    // connect to the socket
    m_subscriber.connect(subscriberAddress.toStdString().c_str());
    if (!m_subscriber.connected())
        qDebug() << QString("Could not connect to the address %1")
                    .arg(subscriberAddress.toStdString().c_str());
    else
        qDebug() << QString("Connected to the address %1")
                    .arg(subscriberAddress.toStdString().c_str());
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

    // receive and recode incoming messages
    while (!m_stopped) {
        if (m_subscriber.connected() && recvMultipart(m_subscriber, name, device, command, data)) {
            qDebug() << "Message received"
                     << QString::fromStdString(name)
                     << QString::fromStdString(device) << QString::fromStdString(command) << QString::fromStdString(data);
            // TODO : analyse the type of the message and to send the
            // corresponding message
        } else {
            // if no data available when we make a pause
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
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
