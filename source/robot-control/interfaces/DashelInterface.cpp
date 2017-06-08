/*
    Aseba - an event-based framework for distributed robot control
    Copyright (C) 2007--2016:
        Stephane Magnenat <stephane at magnenat dot net>
        (http://stephane.magnenat.net)
        and other contributors, see authors.txt for details

    This example is based on a first work of Olivier Marti (2010 - 2011).
    Stripped down & cleaned version by Florian Vaussard (2013).

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

#include "DashelInterface.hpp"

#include <Timer.hpp>

#include <dashel/dashel.h>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QSharedPointer>
#include <QtXml/QDomDocument>

#include <string>
#include <iterator>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>

using namespace Dashel;
using namespace Aseba;

/*!
 * Constructor.
 */
DashelInterface::DashelInterface() :
    NodesManager(),
    m_stream(nullptr),
    m_dashelParams(""),
    m_isRunning(false),
    m_isConnected(false)
{
    qRegisterMetaType<QSharedPointer<Aseba::UserMessage>>("QSharedPointer<Aseba::UserMessage>");
}

/*!
 * Destructor.
 */
DashelInterface::~DashelInterface()
{
    disconnectAseba();
}

/*!
 * Connect to any kind of valid Dashel target (TCP, serial, CAN,...).
 */
void DashelInterface::connectAseba(const QString& dashelTarget)
{
    m_dashelParams = dashelTarget;
    m_isRunning = true;
    start();
}

/*!
 * Connect through a TCP socket.message
 */
void DashelInterface::connectAseba(const QString& ip, const QString& port)
{
    connectAseba("tcp:" + ip + ";port=" + port);
}

/*!
 * Cleanly disconnect.
 */
void DashelInterface::disconnectAseba()
{
    stop();
    wait();
}

/*!
 * Loads the script via dashel.
 */
bool DashelInterface::loadScript(const QString& fileName)
{
    if (!isConnected()) {
        qDebug() << "There is no active connectin, use 'connectAseba'";
        return false;
    }

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << QString("The script file %1 doesn't exist")
                    .arg(fileName);
        return false;
    }

    QDomDocument document("aesl-source");
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!document.setContent(&file, false, &errorMsg, &errorLine, &errorColumn))
    {
        qDebug() << QString("Error in XML source file: %0 at line %1, column %2")
                    .arg(errorMsg).arg(errorLine)
                    .arg(errorColumn);
        return false;
    }

    commonDefinitions.events.clear();
    commonDefinitions.constants.clear();
//    userDefinedVariablesMap.clear();

    int noNodeCount = 0;
    QDomNode domNode = document.documentElement().firstChild();

    // FIXME: this code depends on event and contants being before any code
    bool wasError = false;
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement element = domNode.toElement();
            if (element.tagName() == "node")
            {
                bool ok;
                const unsigned nodeId(getNodeId(element.attribute("name").toStdWString(),
                                                element.attribute("nodeId", 0).toUInt(), &ok));
                if (ok)
                {
                    std::wistringstream is(element.firstChild().toText().data().toStdWString());
                    Error error;
                    BytecodeVector bytecode;
                    unsigned allocatedVariablesCount;

                    Compiler compiler;
                    compiler.setTargetDescription(getDescription(nodeId));
                    compiler.setCommonDefinitions(&commonDefinitions);
                    bool result = compiler.compile(is, bytecode, allocatedVariablesCount, error);

                    if (result)
                    {
                        typedef std::vector<Message*> MessageVector;
                        MessageVector messages;
                        sendBytecode(messages, nodeId, std::vector<uint16>(bytecode.begin(), bytecode.end()));
                        for (MessageVector::const_iterator it = messages.begin(); it != messages.end(); ++it)
                        {
                            sendMessage(**it);
                            delete *it;
                        }
                        Run msg(nodeId);
                        sendMessage(msg);
                    }
                    else
                    {
                        qDebug() << QString::fromStdWString(error.toWString());
                        wasError = true;
                        break;
                    }
//                    // retrieve user-defined variables for use in get/set
//                    userDefinedVariablesMap[element.attribute("name")] = *compiler.getVariablesMap();
                }
                else
                    noNodeCount++;
            }
            else if (element.tagName() == "event")
            {
                const QString eventName(element.attribute("name"));
                const unsigned eventSize(element.attribute("size").toUInt());
                if (eventSize > ASEBA_MAX_EVENT_ARG_SIZE)
                {
                    qDebug() << QString("Event %1 has a length %2 larger than maximum %3")
                                .arg(eventName)
                                .arg(eventSize)
                                .arg(ASEBA_MAX_EVENT_ARG_SIZE);
                    wasError = true;
                    break;
                }
                else
                {
                    commonDefinitions.events.push_back(NamedValue(eventName.toStdWString(), eventSize));
                }
            }
            else if (element.tagName() == "constant")
            {
                commonDefinitions.constants.push_back(NamedValue(element.attribute("name").toStdWString(), element.attribute("value").toInt()));
            }
        }
        domNode = domNode.nextSibling();
    }

    // check if there was an error
    if (wasError)
    {
        qDebug() << QString("There was an error while loading script %1")
                    .arg(fileName);
        commonDefinitions.events.clear();
        commonDefinitions.constants.clear();
//        userDefinedVariablesMap.clear();
    }

    // check if there was some matching problem
    if (noNodeCount)
    {
        qDebug() << QString("%1 scripts have no corresponding nodes in the "
                            "current network and have not been loaded.")
                    .arg(noNodeCount);
    }

    return true;
}

/*!
 * From Dashel::Hub. Message coming from a node. Consider _only_ UserMessage.
 * Discard other types of messages (debug, etc.)
 */
void DashelInterface::incomingData(Dashel::Stream *stream)
{
    try {
        Aseba::Message *message = Aseba::Message::receive(stream);
        // scan this message for nodes descriptions
        Aseba::NodesManager::processMessage(message);

        QSharedPointer<Aseba::UserMessage> userMessage(dynamic_cast<Aseba::UserMessage *>(message));
        if (userMessage)
            emit messageAvailable(userMessage);
    } catch (const DashelException& e) {
        // if this stream has a problem, ignore it for now, and let Hub call
        // connectionClosed later.
        qDebug() << "error while reading message";
    }
}

/*!
 * Send a UserMessage with ID 'id', and optionnally some data values.
 */
void DashelInterface::sendEvent(unsigned id, const Values& values)
{
    if (isConnected())
    {
        Aseba::UserMessage::DataVector data(values.size());
        QListIterator<qint16> it(values);
        unsigned i = 0;
        while (it.hasNext())
            data[i++] = it.next();
        try {
            Aseba::UserMessage(id, data).serialize(m_stream);
            m_stream->flush();
        } catch (const DashelException& e) {
            // if this stream has a problem, ignore it for now, and let Hub call connectionClosed later.
            qDebug() << "Error while writing message";
        }
    }
}

/*!
 * Sends an named event to the robot.
 */
void DashelInterface::sendEventName(const QString& name, const Values& data)
{
    size_t event;
    if (commonDefinitions.events.contains(name.toStdWString(), &event))
        sendEvent(event, data);
    else
        qDebug() << QString("No event named %1").arg(name);
}

/*!
 * Dashel connection was closed.
 */
void DashelInterface::connectionClosed(Dashel::Stream* stream, bool abnormal)
{
    if (abnormal)
        qDebug() << QString("Abnormal connection closed to %1 : %2")
                    .arg(stream->getTargetName().c_str())
                    .arg(stream->getFailReason().c_str());
    else
        qDebug() << QString("Normal connection closed to %1")
                    .arg(stream->getTargetName().c_str());

    emit dashelDisconnection();
    m_isConnected = false;
    m_stream = nullptr;
}

// internals
void DashelInterface::run()
{
    // a timer to stop trying to connect on a timeout
    Timer connectionTimer;
    connectionTimer.reset();
    // try to connect
    while (1) {
        try {
            m_stream = Dashel::Hub::connect(m_dashelParams.toStdString());
            emit dashelConnection();
            m_isConnected = true;
            break;
        } catch (const Dashel::DashelException& e) {
            qDebug()  << "Cannot connect to target: "
                      << m_dashelParams;
            // if can't connect then stop
            if (connectionTimer.isTimedOutSec(10)) {
                qDebug() << "Stop trying to connect to target"
                         << m_dashelParams;
                break;
            } else {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }

    if (isConnected()) {
        QObject::connect(this, &DashelInterface::messageAvailable,
                         this, &DashelInterface::dispatchEvent);
    }

    while (m_isRunning)
        Dashel::Hub::run();

    if (m_stream) {
        connectionClosed(m_stream, false);
        Dashel::Hub::closeStream(m_stream);
    }
}

void DashelInterface::sendMessage(const Aseba::Message& message)
{
    // this is called from the GUI thread through processMessage() or
    // pingNetwork(), so we must lock the Hub before sending
    lock();
    if (isConnected()) {
        try {
            message.serialize(m_stream);
            m_stream->flush();
            unlock();
        } catch (const Dashel::DashelException& e) {
            unlock();
            qDebug() << "Dashel exception: " << e.what();
        }
    } else {
        unlock();
    }
}

void DashelInterface::stop()
{
    m_isRunning = false;
    Dashel::Hub::stop();
}

/*!
 * Flag an event to listen for, and associate callback function (passed by
 * pointer).
 */
void DashelInterface::connectEvent(const QString& eventName, EventCallback callback)
{
    // associate callback with event name
    m_callbacks.insert(std::make_pair(eventName, callback));
}

/*!
 * Callback (slot) used to retrieve subscribed event information.
 */
void DashelInterface::dispatchEvent(QSharedPointer<Aseba::UserMessage> message)
{
    // get name
    QString eventName = QString::fromWCharArray(commonDefinitions.events[message->type].name.c_str());

    // convert values
    Values eventData;
    for (auto& value : message->data)
        eventData.append(value);

 // find and trigger matching callback
    if( m_callbacks.count(eventName) > 0)
    {
        auto eventCallbacks = m_callbacks.equal_range(eventName);
        for (auto iterator = eventCallbacks.first; iterator != eventCallbacks.second; ++iterator)
            (iterator->second)(eventData);
    }
}
