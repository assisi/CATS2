#include "GenericSubscriber.hpp"
#include "zmqHelpers.hpp"

#include <QtCore/QDebug>

#include <thread>

/*!
 * Constructor. Creates the subscriber socket on the provided address.
*/
GenericSubscriber::GenericSubscriber(zmq::context_t& context,
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
GenericSubscriber::~GenericSubscriber()
{
    qDebug() << "Destroying the object";
}

/*!
 * Starts listening.
 */
void GenericSubscriber::process()
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
                // analyse the type of the message and to send the corresponding
                // message
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
void GenericSubscriber::stop()
{
    m_stopped = true;
}
