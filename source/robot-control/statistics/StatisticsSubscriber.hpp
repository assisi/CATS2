#ifndef CATS2_STATISTICS_SUBSCRIBER_HPP
#define CATS2_STATISTICS_SUBSCRIBER_HPP

#include <GenericSubscriber.hpp>

/*!
 * This class accepts the requests for the statistics available and for the
 * list of statistics to provide.
 */
class StaticsticsSubscriber : public GenericSubscriber
{
    Q_OBJECT

public:
    //! Constructor. Makes a subscriber connection to a socket on the provided
    //! address.
    StaticsticsSubscriber(zmq::context_t& context, QStringList subscriberAddresses);
    //! Destructor.
    virtual ~StaticsticsSubscriber();

signals:
    //! Notifies that a get request is received.
    void getStatisticsReceived();
    //! Notifies that a set request is received.
    void postStatisticsReceived(QStringList statisticsIdsList);

protected:
    //! Processes the input message.
    virtual void processMessage(std::string name, std::string device,
                                std::string command, std::string data) override;
};

using StaticsticsSubscriberPtr = QSharedPointer<StaticsticsSubscriber>;

#endif // CATS2_STATISTICS_SUBSCRIBER_HPP
