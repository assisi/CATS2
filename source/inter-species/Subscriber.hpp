#ifndef CATS2_SUBSCRIBER_HPP
#define CATS2_SUBSCRIBER_HPP

#include <GenericSubscriber.hpp>

#include <QtCore/QMap>

/*!
 * Receives messages via zmq, decodes them and sends out qt signals.
 * TODO : to add the reception part
 */
class Subscriber : public GenericSubscriber
{
    Q_OBJECT
public:
    //! Constructor. Creates the subscriber socket on the provided address.
    explicit Subscriber(zmq::context_t& context, QStringList subscriberAddresses);
    //! Destructor.
    virtual ~Subscriber();

signals:
    //! Notifies on the turning direction deduced from the bee setup bees (CW/CCW).
    void notifyBeeSetCircularSetupTurningDirection(QString message);

protected:
    //! Processes the input message.
    virtual void processMessage(std::string name, std::string device,
                                std::string command, std::string data) override;

private: // binary choice experiment related data
    //! The densities of bees around the bee casus.
    QMap<QString, double> m_beeDensities;
};

using SubscriberPtr = QSharedPointer<Subscriber>;

#endif // CATS2_SUBSCRIBER_HPP
