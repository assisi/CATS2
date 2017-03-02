#include "DBusInterface.hpp"

/*!
 * Constructor. Create DBus connection with the interface ch.epfl.mobots.AsebaNetwork.
 */
DBusInterface::DBusInterface() :
    m_bus(QDBusConnection::sessionBus()),
    m_callbacks({}),
    m_dbusMainInterface("ch.epfl.mobots.Aseba", "/", "ch.epfl.mobots.AsebaNetwork",m_bus)
{
    checkConnection();

    // setup event filter
    QDBusMessage eventfilterMessage = m_dbusMainInterface.call("CreateEventFilter");
    QDBusObjectPath eventfilterPath = eventfilterMessage.arguments().at(0).value<QDBusObjectPath>();
    m_eventfilterInterface = new QDBusInterface("ch.epfl.mobots.Aseba", eventfilterPath.path(), "ch.epfl.mobots.EventFilter",m_bus);
    if(!m_bus.connect("ch.epfl.mobots.Aseba",
                    eventfilterPath.path(),
                    "ch.epfl.mobots.EventFilter",
                    "Event",
                    this,
                    SLOT(dispatchEvent(const QDBusMessage&))))
    {
        qDebug() << "failed to connect eventfilter signal to dispatchEvent slot!";
    }
}

/*!
 * Convert  QList<qint16> Values to QVariant.
 */
QVariant DBusInterface::valuetoVariant(const Values& value)
{
    QDBusArgument qdbarg;
    qdbarg << value;
    QVariant qdbvar;
    qdbvar.setValue(qdbarg);
    return qdbvar;
}

/*!
 * Convert  QDBusMessage to QList<qint16> Values, extracting data at a particular
 * index of the construct.
 */
Values DBusInterface::dBusMessagetoValues(const QDBusMessage& dbmess, int index)
{
    QDBusArgument read = dbmess.arguments().at(index).value<QDBusArgument>();
    Values values;
    read >> values;
    return values;
}

/*!
 * Display a list of qint's nicely as a string.
 */
std::string DBusInterface::toString(const Values& v)
{
    std::string out = "[";
    for (int i = 0; i < v.size(); i++)
    {
        out += std::to_string(v[i]);
        if(i <(v.size()-1))
        {
            out += ",";
        }
    }
    out += "]";
    return out;
}

/*!
 * Check if the connection was estalished.
 */
bool DBusInterface::checkConnection()
{
    if (!QDBusConnection::sessionBus().isConnected())
    {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                        "To start it, run:\n"
                        "\teval `dbus-launch --auto-syntax`\n");
        qDebug() << "error";
        return false;
    }
    qDebug() << "You are connected to the D-Bus session bus";

    QDBusMessage nodelist = m_dbusMainInterface.call("GetNodesList");

    for (int i = 0; i < nodelist.arguments().size(); ++i)
    {
        m_nodeList << nodelist.arguments().at(i).value<QString>();
    }

    displayNodeList();

    return true;
}

/*!
 * Display the list of Aseba Nodes connected on the Aseba Network.
 */
void DBusInterface::displayNodeList()
{
    qDebug() << "Aseba nodes list: ";
    qDebug() << m_nodeList;
}

/*!
 * Load an Aseba script (.aesl) on an Aseba Node.
 */
void DBusInterface::loadScript(const QString& script)
{
    // check to ensure that we can find the file
    QFile file(script);
    QFileInfo fi(script);
    // include absolute path
    QString path_to_script = fi.absoluteFilePath();
    if(!file.exists())
    {
        qDebug() << "Cannot find file: " << path_to_script << ".";
    }
    else
    {
        // load the script
        m_dbusMainInterface.call("LoadScripts",path_to_script);
    }
}

/*!
 * Get an Aseba variable from a Aseba node.
 */
Values DBusInterface::getVariable(const QString& node, const QString& variable)
{
    return dBusMessagetoValues(m_dbusMainInterface.call( "GetVariable",node, variable),0);
}

/*!
 * Set an Aseba variable from a Aseba node.
 */
void DBusInterface::setVariable(const QString& node, const QString& variable, const Values& value)
{
    m_dbusMainInterface.call("SetVariable", node, variable, valuetoVariant(value));
}

/*!
 * Flag an event to listen for, and associate callback function (passed by
 * pointer).
 */
void DBusInterface::connectEvent(const QString& eventName, EventCallback callback)
{
    // associate callback with event name
    m_callbacks.insert(std::make_pair(eventName, callback));

    // listen
    m_eventfilterInterface->call("ListenEventName", eventName);
}

/*!
 * Send Aseba Event using the ID of the Event.
 */
void DBusInterface::sendEvent(quint16 eventID, const Values& value)
{
    QDBusArgument argument;
    argument << eventID;
    QVariant variant;
    variant.setValue(argument);
    m_dbusMainInterface.call("SendEvent", variant, valuetoVariant(value));
}

/*!
 * Send Aseba Event using the name of the Event.
 */
void DBusInterface::sendEventName(const QString& eventName, const Values& value)
{
    m_dbusMainInterface.call("SendEventName", eventName, valuetoVariant(value));
}

/*!
 * Callback (slot) used to retrieve subscribed event information.
 */
void DBusInterface::dispatchEvent(const QDBusMessage& message)
{
    // unpack event
    QString eventReceivedName= message.arguments().at(1).value<QString>();
    Values eventReceivedValues = dBusMessagetoValues(message,2);
    // find and trigger matching callback
    if( m_callbacks.count(eventReceivedName) > 0)
    {
        auto eventCallbacks = m_callbacks.equal_range(eventReceivedName);
        for (auto iterator = eventCallbacks.first; iterator != eventCallbacks.second; ++iterator)
            (iterator->second)(eventReceivedValues);
    }
}
