#include "Receiver.hpp"
#include "zmqHelpers.hpp"

#include <QtCore/QDebug>

#include <thread>

/*!
 * Constructor. Makes a subscriber connection to a socket on the provided
 * address. The socket is bound.
*/
Receiver::Receiver(zmq::context_t& context, QStringList addresses):
    GenericSubscriber(context, addresses, true)
{

}

/*!
* Destructor.
*/
Receiver::~Receiver()
{
    qDebug() << "Destroying the object";
}

/*!
 * Processes the input message.
 */
void Receiver::processMessage(std::string name, std::string device,
                                std::string command, std::string data)
{
    if (command.compare("set") == 0) {
        emit setRequestReceived(device, std::stod(data));
    } else if (command.compare("get") == 0) {
        emit getRequestReceived(device);
    }
}
