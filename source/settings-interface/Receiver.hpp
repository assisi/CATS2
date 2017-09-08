#ifndef CATS2_RECEIVER_HPP
#define CATS2_RECEIVER_HPP

#include <GenericSubscriber.hpp>

/*!
 * Receives set/get requests via zmq, decodes them and sends out qt signals.
 */
class Receiver : public GenericSubscriber
{
    Q_OBJECT

public:
    //! Constructor. Makes a subscriber connection to a socket on the provided
    //! address.
    Receiver(zmq::context_t& context, QStringList subscriberAddresses);
    //! Destructor.
    virtual ~Receiver();

signals:
    //! Notifies that a get request is received.
    void getRequestReceived(std::string path);
    //! Notifies that a set request is received.
    void setRequestReceived(std::string path, std::vector<double> values);

protected:
    //! Processes the input message.
    virtual void processMessage(std::string name, std::string device,
                                std::string command, std::string data) override;

private:
    //! Converts the string to the data vector.
    std::vector<double> parseDataString(std::string dataString);
};

using ReceiverPtr = QSharedPointer<Receiver>;

#endif // CATS2_RECEIVER_HPP
