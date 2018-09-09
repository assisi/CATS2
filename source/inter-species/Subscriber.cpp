#include "Subscriber.hpp"
#include "zmqHelpers.hpp"

#include <QtCore/QDebug>

#include <thread>

/*!
 * Constructor. Creates the subscriber socket on the provided address.
*/
Subscriber::Subscriber(zmq::context_t& context,
                       QStringList subscriberAddresses):
    //GenericSubscriber(context, subscriberAddresses)
    GenericSubscriber(context, subscriberAddresses, false) // XXX Does not work with "connect", use "bind" instead
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
//    // at the moment we manage the binary-choice message only
//    if (QString(command.data()).toLower().contains("casu")) {
//        if (QString(device.data()).toLower().contains("message")) {
//            m_beeDensities[QString(command.data()).toLower()] =
//                    QString(data.data()).toDouble();
//
//            // by agreement when we get the data from the second casu we decide
//            // on the turning direction
//            if (QString(command.data()).toLower().contains("002")) {
//                QString message;
//                if (m_beeDensities["casu-001"] < m_beeDensities["casu-002"])
//                    message = "CW";
//                else
//                    message = "CCW";
//                emit notifyBeeSetCircularSetupTurningDirection(message);
//            }
//        }
//    }


//    if (QString(device.data()).toLower() == "proberq") {
//        if (QString(data.data()).toLower().contains("confidence")) {
//            QStringList splitted = QString(data.data()).split(":");
//            float confidence = splitted.value(splitted.length() - 1).toFloat();
//            if (confidence < 0.0)
//                confidence = 0.0;
//            else if (confidence > 1.0)
//                confidence = 1.0;
//        }
//    }


        if (QString(device.data()).toLower() == "behaviour") {
            // SetBehaviour packets
            emit notifyBeeSetCircularSetupTurningDirection(QString(data.data()));
        } else if (QString(device.data()).toLower() == "robottargetpositionchanged") {
            // Set robot target position packets
            emit notifyReceptionOfUpdateRobotTargetPositionMessage(QString(data.data()));
        } else if (QString(device.data()).toLower() == "behaviour2r") {
            emit notifyInterspecies2RoomsModeChange(QString(data.data()));
        } else if (QString(device.data()).toLower() == "stop") {
            emit notifyStopAllRobots();
        }


}
