#include "Subscriber.hpp"
#include "zmqHelpers.hpp"

#include <QtCore/QDebug>

#include <thread>

/*!
 * Constructor. Creates the subscriber socket on the provided address.
*/
Subscriber::Subscriber(zmq::context_t& context,
                       QStringList subscriberAddresses):
    GenericSubscriber(context, subscriberAddresses)
{

}

/*!
* Destructor.
*/
Subscriber::~Subscriber()
{
    qDebug() << "Destroying the object";
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
