#include "StatisticsSubscriber.hpp"
#include "zmqHelpers.hpp"

#include <QtCore/QDebug>

#include <thread>

/*!
 * Constructor. Makes a subscriber connection to a socket on the provided
 * address. The socket is bound.
*/
StaticsticsSubscriber::StaticsticsSubscriber(zmq::context_t& context,
                                             QStringList addresses):
    GenericSubscriber(context, addresses, true)
{

}

/*!
* Destructor.
*/
StaticsticsSubscriber::~StaticsticsSubscriber()
{
    qDebug() << "Destroying the object";
}

/*!
 * Processes the input message.
 */
void StaticsticsSubscriber::processMessage(std::string name, std::string device,
                                           std::string command, std::string data)
{
    if (command.compare("get-statistics") == 0) {
        emit getStatisticsReceived();
    } else if (command.compare("post") == 0) {
        QStringList statisticsIdsList = QString::fromStdString(data).split(';');
        emit postStatisticsReceived(statisticsIdsList);
    }
}
