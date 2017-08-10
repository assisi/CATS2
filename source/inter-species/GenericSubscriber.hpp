#ifndef CATS2_GENERIC_SUBSCRIBER_HPP
#define CATS2_GENERIC_SUBSCRIBER_HPP

#include <zmq.hpp>

#include <QtCore/QObject>

#include <atomic>

/*!
 * Receives messages via zmq, decodes them and sends out qt signals.
 * TODO : to add the reception part
 */
class GenericSubscriber : public QObject
{
    Q_OBJECT
public:
    //! Constructor. Creates the subscriber socket on the provided address.
    GenericSubscriber(zmq::context_t& context, QStringList subscriberAddresses);
    //! Destructor.
    virtual ~GenericSubscriber();

signals:
    //! Notifies that the subscriber is stopped.
    void finished();
    //! Notifies about an error.
    void error(QString errorMessage);

public slots:
    //! Starts listening.
    void process();
    //! Stops listening.
    void stop();

protected:
    //! Processes the input message.
    virtual void processMessage(std::string name, std::string device,
                                std::string command, std::string data) = 0;

private:
    //! The subscriber socket.
    zmq::socket_t m_subscriber;

    //! The flag that defines if the convertor is to be stopped.
    std::atomic_bool m_stopped;
};

#endif // CATS2_GENERIC_SUBSCRIBER_HPP
