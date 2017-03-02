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

#include <libxml/parser.h>
#include <libxml/tree.h>

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
bool DashelInterface::loadScript(const QString& asebaScript)
{
    if (!m_isConnected) {
        qDebug() << Q_FUNC_INFO << "There is no active connectin, use "
                                   "'connectAseba'";
        return false;
    }

    std::string script = asebaScript.toStdString();
    xmlDoc *doc = xmlReadFile(script.c_str(), NULL, 0);
    if (!doc) {
        qDebug() << Q_FUNC_INFO
                 << QString("Cannot read/find the file %1").arg(script.c_str());
        return false;
    }
    if(!m_stream) {
        qDebug() << Q_FUNC_INFO << "Target not detected, no script can be loaded";
        return false;
    }
    xmlNode *domRoot = xmlDocGetRootElement(doc);

    commonDefinitions.events.clear();
    commonDefinitions.constants.clear();
    allVariables.clear();

    int noNodeCount = 0;
    if (!xmlStrEqual(domRoot->name, BAD_CAST("network"))) {
        qDebug() << Q_FUNC_INFO
                 << "root node is not \"network\", XML considered as invalid";
        return false;
    } else {
        for (xmlNode *domNode = xmlFirstElementChild(domRoot); domNode; domNode = domNode->next) {
            if (domNode->type == XML_ELEMENT_NODE)  {
                if (xmlStrEqual(domNode->name, BAD_CAST("node"))) {
                    xmlChar *name = xmlGetProp(domNode, BAD_CAST("name"));
                    if (!name) {
                        qDebug() << Q_FUNC_INFO
                                 << "missing \"name\" attribute in \"node\" entry";
                    } else {
                        const string _name((const char *)name);
                        xmlChar * text = xmlNodeGetContent(domNode);
                        if (!text) {
                            qDebug() << Q_FUNC_INFO
                                     <<"missing text in \"node\" entry";
                        } else {
                            unsigned preferedId(0);
                            xmlChar *storedId = xmlGetProp(domNode, BAD_CAST("nodeId"));
                            if (storedId) {
                                preferedId = unsigned(atoi((char*)storedId));
                            }
                            bool ok;

                            //get description of the node in order (Mandatory!!!)
                            GetDescription getDes;
                            getDes.serialize(m_stream);
                            m_stream->flush();

                            sleep(1); //Need to wait 1 sec to receive the description of the node

                            //Call Description Manager for the Target description
                            unsigned nodeId(getNodeId(widen(_name), preferedId, &ok));
                            if (ok) {
                                std::wistringstream is(widen((const char *)text));
                                Error error;
                                BytecodeVector bytecode;
                                unsigned allocatedVariablesCount;

                                //Instance compiler to load the code using description manager
                                // -> common/msg/descriptions-manager.h/.cpp
                                Compiler compiler;
                                compiler.setTargetDescription(getDescription(nodeId));
                                compiler.setCommonDefinitions(&commonDefinitions);
                                bool result = compiler.compile(is, bytecode, allocatedVariablesCount, error);

                                if (result) {
                                    sendBytecode(m_stream, nodeId, std::vector<uint16>(bytecode.begin(), bytecode.end()));
                                    Run msg(nodeId); //Run the Aseba script
//                                    msg.serialize(m_stream);
//                                    m_stream->flush();
                                    sendMessage(msg);
                                    qDebug() << Q_FUNC_INFO
                                             << QString("The Script %1 has been "
                                                        "loaded into node %2. "
                                                        "Enjoy!")
                                                .arg(script.c_str())
                                                .arg(_name.c_str());
                                } else {
                                    qDebug() << Q_FUNC_INFO << "compilation failed";
                                    return false;
                                }
                            } else {
                                noNodeCount++;
                            }
                            xmlFree(text);
                        }
                        xmlFree(name);
                    }
                } else if (xmlStrEqual(domNode->name, BAD_CAST("event"))) {
                    // get attributes
                    xmlChar *name = xmlGetProp(domNode, BAD_CAST("name"));
                    if (!name)
                        qDebug() << Q_FUNC_INFO << "missing event";
                    xmlChar *size = xmlGetProp(domNode, BAD_CAST("size"));
                    if (!size)
                        qDebug() << Q_FUNC_INFO << "missing size";
                    // add event
                    if (name && size) {
                        int eventSize(atoi((const char *)size));
                        if (eventSize > ASEBA_MAX_EVENT_ARG_SIZE) {
                            qDebug() << Q_FUNC_INFO << "event too big";
                            break;
                        } else {
                            commonDefinitions.events.push_back(NamedValue(widen((const char *)name), eventSize));
                        }
                    }
                    // free attributes
                    if (name)
                        xmlFree(name);
                    if (size)
                        xmlFree(size);
                }
                else if (xmlStrEqual(domNode->name, BAD_CAST("constant"))) {
                    // get attributes
                    xmlChar *name = xmlGetProp(domNode, BAD_CAST("name"));
                    if (!name)
                        qDebug() << Q_FUNC_INFO << "Missing event";
                    xmlChar *value = xmlGetProp(domNode, BAD_CAST("value"));
                    if (!value)
                        qDebug() << Q_FUNC_INFO << "Missing constant";
                    // add constant if attributes are valid
                    if (name && value) {
                        commonDefinitions.constants.push_back(NamedValue(widen((const char *)name), atoi((const char *)value)));
                    }
                    // free attributes
                    if (name)
                        xmlFree(name);
                    if (value)
                        xmlFree(value);
                } else {
                    qDebug() << Q_FUNC_INFO << "Unknown XML node seen in .aesl file";
                }
            }

        }
    }
    xmlFreeDoc(doc);
    return true;
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
void DashelInterface::sendEvent(unsigned id, const QVector<int>& values)
{
    if (this->m_isConnected)
    {
        Aseba::UserMessage::DataVector data(values.size());
        QVectorIterator<int> it(values);
        unsigned i = 0;
        while (it.hasNext())
            data[i++] = it.next();
        Aseba::UserMessage(id, data).serialize(m_stream);
        m_stream->flush();
    }
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
    while (1)
    {
        try
        {
            m_stream = Dashel::Hub::connect(m_dashelParams.toStdString());

            emit dashelConnection();
            qDebug() << "Connected to target: " << m_dashelParams;
            m_isConnected = true;
            break;
        }
        catch (Dashel::DashelException e)
        {
            qDebug() << "Cannot connect to target: " << m_dashelParams;
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
    if (m_stream)
    {
        try
        {
            message.serialize(m_stream);
            m_stream->flush();
            unlock();
        }
        catch(Dashel::DashelException e)
        {
            unlock();
            qDebug() << Q_FUNC_INFO << "Dashel exception: " << e.what();
        }
    }
    else
    {
        unlock();
    }
}

void DashelInterface::stop()
{
    m_isRunning = false;
    Dashel::Hub::stop();
}
