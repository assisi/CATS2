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

#include <QtCore/QDebug>
#include <dashel/dashel.h>
#include <string>
#include <iterator>
#include <cstdlib>
#include <iostream>

#include <QtCore/QFile>
#include <QtXml/QDomDocument>

using namespace std;
using namespace Dashel;
using namespace Aseba;

/*!
 * Constructor.
 */
DashelInterface::DashelInterface() : m_isRunning(false), m_isConnected(false),
                                     NodesManager(),
                                     m_dashelParams("")
{

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
    if (!m_isConnected) {
        qDebug() << Q_FUNC_INFO << "There is no active connectin, use "
                                   "'connectAseba'";
        return false;
    }

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << Q_FUNC_INFO
                 << QString("The script file %1 doesn't exist")
                    .arg(fileName);
        return false;
    }

    QDomDocument document("aesl-source");
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!document.setContent(&file, false, &errorMsg, &errorLine, &errorColumn))
    {
        qDebug() << Q_FUNC_INFO
                 << QString("Error in XML source file: %0 at line %1, column %2")
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
                        qDebug() << Q_FUNC_INFO
                                 << QString::fromStdWString(error.toWString());
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
                    qDebug() << Q_FUNC_INFO
                             << QString("Event %1 has a length %2 larger than maximum %3")
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
        qDebug() << Q_FUNC_INFO
                 << QString("There was an error while loading script %1")
                    .arg(fileName);
        commonDefinitions.events.clear();
        commonDefinitions.constants.clear();
//        userDefinedVariablesMap.clear();
    }

    // check if there was some matching problem
    if (noNodeCount)
    {
        qDebug() << Q_FUNC_INFO
                 << QString("%1 scripts have no corresponding nodes in the "
                            "current network and have not been loaded.")
                    .arg(noNodeCount);
    }
}

/*!
 * From Dashel::Hub. Message coming from a node. Consider _only_ UserMessage.
 * Discard other types of messages (debug, etc.)
 */
void DashelInterface::incomingData(Dashel::Stream *stream)
{
    Aseba::Message *message = Aseba::Message::receive(stream);
    Aseba::UserMessage *userMessage = dynamic_cast<Aseba::UserMessage *>(message);

    //A description manager for loading the Aseba Scripts
    Aseba::NodesManager::processMessage(message);

    if (userMessage)
        emit messageAvailable(userMessage);
    else
        delete message;
}

/*!
 * Send a UserMessage with ID 'id', and optionnally some data values.
 */
void DashelInterface::sendEvent(unsigned id, const Values& values)
{
    if (this->m_isConnected)
    {
        Aseba::UserMessage::DataVector data(values.size());
        QListIterator<qint16> it(values);
        unsigned i = 0;
        while (it.hasNext())
            data[i++] = it.next();
        Aseba::UserMessage(id, data).serialize(m_stream);
        m_stream->flush();
    }
}

void DashelInterface::sendEventName(const QString& name, const Values& data)
{
    size_t event;
    if (commonDefinitions.events.contains(name.toStdWString(), &event))
        sendEvent(event, data);
    else
        qDebug() << Q_FUNC_INFO
                 << QString("No event named %1").arg(name);
}

/*!
 * Dashel connection was closed.
 */
void DashelInterface::connectionClosed(Dashel::Stream* stream, bool abnormal)
{
    Q_UNUSED(stream);
    Q_UNUSED(abnormal);
    emit dashelDisconnection();
    m_stream = 0;
}

// internals
void DashelInterface::run()
{
    while (1) {
        try {
            m_stream = Dashel::Hub::connect(m_dashelParams.toStdString());

            emit dashelConnection();
            qDebug()  << Q_FUNC_INFO
                      << "Connected to target: " << m_dashelParams;
            m_isConnected = true;
            break;
        } catch (Dashel::DashelException e) {
            qDebug()  << Q_FUNC_INFO
                      << "Cannot connect to target: " << m_dashelParams;
            sleep(1000000L); // 1s
        }
    }

    while (m_isRunning)
        Dashel::Hub::run();
}

// UTF8 to wstring
std::wstring DashelInterface::widen(const char *src)
{
    const size_t destSize(mbstowcs(0, src, 0)+1);
    std::vector<wchar_t> buffer(destSize, 0);
    mbstowcs(&buffer[0], src, destSize);
    return std::wstring(buffer.begin(), buffer.end() - 1);
}

std::wstring DashelInterface::widen(const std::string& src)
{
    return widen(src.c_str());
}

std::string DashelInterface::narrow(const wchar_t* src)
{
    const size_t destSize(wcstombs(0, src, 0)+1);
    std::vector<char> buffer(destSize, 0);
    wcstombs(&buffer[0], src, destSize);
    return std::string(buffer.begin(), buffer.end() - 1);
}

std::string DashelInterface::narrow(const std::wstring& src)
{
    return narrow(src.c_str());
}

void DashelInterface::sendMessage(const Aseba::Message& message)
{
    // this is called from the GUI thread through processMessage() or pingNetwork(), so we must lock the Hub before sending
    lock();
    if (m_stream) {
        try {
            message.serialize(m_stream);
            m_stream->flush();
            unlock();
        } catch(Dashel::DashelException e) {
            unlock();
            qDebug() << Q_FUNC_INFO << "Dashel exception: " << e.what();
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


