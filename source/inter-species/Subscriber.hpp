#ifndef CATS2_SUBSCRIBER_HPP
#define CATS2_SUBSCRIBER_HPP

#include <zmq.hpp>

#include <QtCore/QObject>

#include <atomic>

/*!
 * Receives messages via zmq, decodes them and sends out qt signals.
 * TODO : to add the reception part
 */
class Subscriber : public QObject
{
    Q_OBJECT
public:
    //! Constructor. Creates the subscriber socket on the provided address.
    Subscriber(zmq::context_t& context, QStringList subscriberAddresses);
    //! Destructor.
    virtual ~Subscriber();

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

private:
    //! The subscriber socket.
    zmq::socket_t m_subscriber;

    //! The flag that defines if the convertor is to be stopped.
    std::atomic_bool m_stopped;
};

using SubscriberPtr = QSharedPointer<Subscriber>;

#endif // CATS2_SUBSCRIBER_HPP
