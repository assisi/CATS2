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
    // TODO : to implement
}
