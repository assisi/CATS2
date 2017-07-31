#ifndef CATS2_SUBSCRIBER_HPP
#define CATS2_SUBSCRIBER_HPP

#include <GenericSubscriber.hpp>

/*!
 * Receives messages via zmq, decodes them and sends out qt signals.
 * TODO : to add the reception part
 */
class Subscriber : public GenericSubscriber
{
    Q_OBJECT
public:
    //! Constructor. Creates the subscriber socket on the provided address.
    Subscriber(zmq::context_t& context, QStringList subscriberAddresses);
    //! Destructor.
    virtual ~Subscriber();

protected:
    //! Processes the input message.
    virtual void processMessage(std::string name, std::string device,
                                std::string command, std::string data) override;
};

using SubscriberPtr = QSharedPointer<Subscriber>;

#endif // CATS2_SUBSCRIBER_HPP
