/*
    A Qt binding for Aseba that relies on DBus

    Supports event sending/receiving, Aseba scripts Loading, as well as setting/getting an Aseba variable.

    Authors: Frank Bonnet, Stefan Witwicki
    Copyright (C) 2007--2016

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CATS2_DBUS_INTERFACE_HPP
#define CATS2_DBUS_INTERFACE_HPP

#include <QtDBus/QtDBus>
#include <functional>

Q_DECLARE_METATYPE(QList<qint16>);


typedef QList<qint16> Values;

/*!
 * This class is a re-edited version of DBusInterface from aseba/examples/clients
 * (https://github.com/aseba-community/aseba)
 */
class DBusInterface : public QObject
{
    Q_OBJECT

public:
    //! Constructor.
    DBusInterface();

    //! Convert  QList<qint16> Values to QVariant.
    static QVariant valuetoVariant(const Values& value);
    //! Convert  QDBusMessage to QList<qint16> Values, extracting data at a
    //! particular index of the construct.
    static Values dBusMessagetoValues(const QDBusMessage& dbmess, int index);
    //! Display a list of qint's nicely as a string.
    static std::string toString(const Values& v);

    //! Check if the connection was estalished.
    bool checkConnection();
    //! Display the list of Aseba Nodes connected on the Aseba Network.
    void displayNodeList();
    //! Load an Aseba script (.aesl) on an Aseba Node.
    void loadScript(const QString& script);
    //! Get an Aseba variable from a Aseba node.
    Values getVariable(const QString& node, const QString& variable);
    //! Set an Aseba variable from a Aseba node.
    void setVariable(const QString& node, const QString& variable, const Values& value);


    typedef std::function<void(const Values&)> EventCallback;
    //! Flag an event to listen for, and associate callback function
    //! (passed by pointer).
    void connectEvent(const QString& eventName, EventCallback callback);

    //! Send Aseba Event using the ID of the Event.
    void sendEvent(quint16 eventID, const Values& value);
    //! Send Aseba Event using the name of the Event.
    void sendEventName(const QString& eventName, const Values& value);

public slots:
    //! Callback (slot) used to retrieve subscribed event information.
    void dispatchEvent(const QDBusMessage& message);

private:
    QList<QString> m_nodeList;
    QDBusConnection m_bus;
    std::multimap<QString, EventCallback> m_callbacks;
    QDBusInterface m_dbusMainInterface;
    QDBusInterface* m_eventfilterInterface;
};

#endif // CATS2_DBUS_INTERFACE_HPP
