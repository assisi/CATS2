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

/*!
 * This class is a re-edited version of DashelInterface from aseba/examples/clients
 * (https://github.com/aseba-community/aseba)
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

    //! Send a UserMessage with ID 'id', and optionnally some data values.
    void sendEvent(unsigned id, const Values& data = Values());
    void sendEventName(const QString& name, const Values& data);

    //! From Dashel::Hub
    virtual void stop();

signals:
    void messageAvailable(Aseba::UserMessage *message);
    void dashelDisconnection();
    void dashelConnection();

protected:
    Dashel::Stream* targetStream;
    // from QThread
    virtual void run() override;

    //! From Dashel::Hub. Message coming from a node. Consider _only_
    //! UserMessage. Discard other types of messages (debug, etc.)
    virtual void incomingData(Dashel::Stream *m_stream) override;
    //! Dashel connection was closed.
    virtual void connectionClosed(Dashel::Stream *m_stream, bool abnormal) override;

public:
    //! Redefined from NodesManager.
    virtual void sendMessage(const Aseba::Message& message);

protected:
    typedef std::vector<std::string> strings;
    typedef std::map<std::string, Aseba::VariablesMap> NodeNameToVariablesMap;

    // members
    Dashel::Stream* m_stream;
    QString m_dashelParams;
    bool m_isRunning;
    bool m_isConnected;

    Aseba::CommonDefinitions commonDefinitions;
    NodeNameToVariablesMap allVariables;
//    typedef std::map<std::wstring, std::pair<unsigned, unsigned> > VariablesMap;
//    typedef QMap<QString, VariablesMap> UserDefinedVariablesMap;
//    UserDefinedVariablesMap userDefinedVariablesMap;

private:
    // string operations
    static std::wstring widen(const char *src);
    static std::wstring widen(const std::string& src);
    static std::string narrow(const wchar_t* src);
    static std::string narrow(const std::wstring& src);
};

#endif // CATS2_DASHEL_INTERFACE_HPP
