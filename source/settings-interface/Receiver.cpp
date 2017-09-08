#include "Receiver.hpp"
#include "zmqHelpers.hpp"

#include <QtCore/QDebug>

#include <boost/algorithm/string.hpp>

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
        emit setRequestReceived(device, parseDataString(data));
    } else if (command.compare("get") == 0) {
        emit getRequestReceived(device);
    }
}

/*!
 * Converts the string to the data vector.
 */
std::vector<double> Receiver::parseDataString(std::string dataString)
{
    std::vector<double> values;
    std::vector<std::string> valueStrings;
    boost::split(valueStrings, dataString, [](char c){return c == ';';});
    for (const std::string& valueString : valueStrings) {
        values.push_back(std::stod(valueString));
    }
    return values;
}
