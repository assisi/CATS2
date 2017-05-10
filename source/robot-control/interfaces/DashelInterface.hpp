#ifndef CATS2_DASHEL_INTERFACE_HPP
#define CATS2_DASHEL_INTERFACE_HPP

#include "Values.hpp"

#include <QtCore/QThread>
#include <QtCore/QVector>
#include <QtCore/QString>

#include <dashel/dashel.h>

// There was a conflict between uint64 from aseba and OpenCV
// HACK : stackoverflow.com/questions/8865744/avoiding-conflicting-declaration-errors-in-typedef-c
#define uint64 auint64
#include <common/msg/msg.h>
#undef uint64
#include <common/msg/NodesManager.h>

#include <string>
#include <map>
#include <atomic>

/*!
 * This class is a re-edited version of DashelInterface from aseba/examples/clients
 * (https://github.com/aseba-community/aseba)
 * FIXME : using threads like this is a bad design and all slots will be called in the
 * original thread, need to redo this properly (http://doc.qt.io/qt-4.8/qthread.html)
 */
class DashelInterface : public QThread, public Dashel::Hub, public Aseba::NodesManager
{
    Q_OBJECT

public:
    //! Constructor.
    explicit DashelInterface();
    //! Destructor.
    virtual ~DashelInterface();

    //! Connect to any kind of valid Dashel target (TCP, serial, CAN,...).
    void connectAseba(const QString& dashelTarget);
    //! Connect through a TCP socket.
    void connectAseba(const QString& ip, const QString& port);
    //! Loads the script via dashel.
    bool loadScript(const QString& asebaScript);
    //! Cleanly disconnect.
    void disconnectAseba();

    //! Stops the hub
    void stop();

public:
    //! Redefined from NodesManager.
    virtual void sendMessage(const Aseba::Message& message) override;

public:
    //! Returns the connection status flag.
    bool isConnected() const { return m_isConnected && m_isRunning; }

public:
    //! Sends an named event to the robot.
    void sendEventName(const QString& name, const Values& data);

    //! Flag an event to listen for, and associate callback function
    //! (passed by pointer).
    void connectEvent(const QString& eventName, EventCallback callback);

signals:
    void messageAvailable(QSharedPointer<Aseba::UserMessage> message);
    void dashelDisconnection();
    void dashelConnection();

protected slots:
    //! Callback (slot) used to retrieve subscribed event information.
    void dispatchEvent(QSharedPointer<Aseba::UserMessage> message);

protected:
    //! Send a UserMessage with ID 'id', and optionnally some data values.
    void sendEvent(unsigned id, const Values& data = Values());

protected:
    virtual void run() override;

    //! From Dashel::Hub. Message coming from a node. Consider _only_
    //! UserMessage. Discard other types of messages (debug, etc.)
    virtual void incomingData(Dashel::Stream *m_stream) override;
    //! Dashel connection was closed.
    virtual void connectionClosed(Dashel::Stream *m_stream, bool abnormal) override;

protected:
    typedef std::vector<std::string> strings;
    typedef std::map<std::string, Aseba::VariablesMap> NodeNameToVariablesMap;

    //! Callbacks to be triggered when an event arrives.
    std::multimap<QString, EventCallback> m_callbacks;

    // members
    Dashel::Stream* m_stream;
    QString m_dashelParams;
    bool m_isRunning;
    std::atomic_bool m_isConnected;

    Aseba::CommonDefinitions commonDefinitions;
    NodeNameToVariablesMap allVariables;
};

#endif // CATS2_DASHEL_INTERFACE_HPP
