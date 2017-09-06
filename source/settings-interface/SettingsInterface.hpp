#ifndef CATS2_SETTINGS_INTERFACE_HPP
#define CATS2_SETTINGS_INTERFACE_HPP

#include "Receiver.hpp"

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>

/*!
 * Provides the interface between the application settings and external world.
 * This allows to set them from the third-party applications.
 */
class SettingsInterface : public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit SettingsInterface();
    //! Destructor.
    virtual ~SettingsInterface();

public slots:
    //! Processes input set messages.
    void onSetRequestReceived(std::string path, std::vector<double> values);
    //! Processes input get messages.
    void onGetRequestReceived(std::string path);

private:
    //! Sends a message.
    void sendMessage(std::string& name, std::string& device,
                     std::string& desc, std::string& data);

private:
    //! The zmq context, one should use exactly one context in a process.
    zmq::context_t m_context;
    //! The publisher socket.
    zmq::socket_t m_sender;
    //! The class receiving the data from the remote system.
    ReceiverPtr m_receiver;

    // TODO : put these data to settings
    //! The receiver address.
    static constexpr char ReceiverAddress[] = "tcp://127.0.0.1:5557";
    //! The sender address.
    static constexpr char SenderAddress[] = "tcp://127.0.0.1:5558";
};

using SettingsInterfacePtr = QSharedPointer<SettingsInterface>;

#endif // CATS2_SETTINGS_INTERFACE_HPP
