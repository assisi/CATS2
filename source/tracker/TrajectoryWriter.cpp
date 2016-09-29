#include "TrajectoryWriter.hpp"

#include "settings/TrackingSettings.hpp"

#include <RunTimer.h>

#include <QtCore/QDebug>
#include <QtCore/QPair>
#include <QtWidgets/QApplication>
#include <QtCore/QDir>

#include <QtCore/QStandardPaths>

/*!
 * Constructor.
 */
TrajectoryWriter::TrajectoryWriter()
{
    // create the output folder
    QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).mkdir(QApplication::applicationName());

    QString filePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
                       QDir::separator() + QApplication::applicationName();
    QString fileName = QString("%1 %2.txt").arg(TrackingSettings::get().experimentName())
                                           .arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
    // open the text where to write the tracking results
    m_resultsFile.setFileName(filePath + QDir::separator() + fileName);
    if (m_resultsFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_resultsStream.setDevice(&m_resultsFile);
        // write the header
        m_resultsStream << "timeStep" << "\t";
        for(int i = 0; i < TrackingSettings::get().numberOfRobots(); ++i) {
            m_resultsStream << "robot" << i << "X" << "\t";
            m_resultsStream << "robot" << i << "Y" << "\t";
            m_resultsStream << "robot" << i << "Direction" << "\t";
        }
        for(int i = 0; i < TrackingSettings::get().numberOfAnimals(); ++i) {
            m_resultsStream << "fish" << i << "X" << "\t";
            m_resultsStream << "fish" << i << "Y" << "\t";
            m_resultsStream << "fish" << i << "Direction" << "\t";
        }
        m_resultsStream << endl;
    }
}

/*!
 * Destructor.
 */
TrajectoryWriter::~TrajectoryWriter()
{
    // FIXME : looks like this destructor is never called. To find why
    m_resultsFile.close();
}

/*!
 * Saves the tracking results to the ouptup file.
 */
void TrajectoryWriter::writeData(std::chrono::milliseconds timestamp, const QList<AgentDataWorld>& agentsData)
{
    if (!m_resultsFile.isOpen())
        return;

    // first write the timestamp converted to seconds
    double timeFromStartSec = RunTimer::get().runtimeSecTo(timestamp);
    m_resultsStream << QString::number(timeFromStartSec, 'f', 3) << "\t";

    // first write all the robots
    for(int i = 0; i < TrackingSettings::get().numberOfRobots(); ++i) {
        const AgentDataWorld* robotData = getAgentData(i, AgentType::FISH_CASU, m_robotsIndexToId, agentsData);
        if (robotData) {
            m_resultsStream << robotData->state().position().x() << "\t";
            m_resultsStream << robotData->state().position().y() << "\t";
            m_resultsStream << robotData->state().orientation().angle() << "\t";
        } else {
            m_resultsStream << "NaN\t";
            m_resultsStream << "NaN\t";
            m_resultsStream << "NaN\t";
        }
    }

    // then write the fish
    for(int i = 0; i < TrackingSettings::get().numberOfAnimals(); ++i) {
        const AgentDataWorld* animalData = getAgentData(i, AgentType::FISH, m_fishIndexToId, agentsData);
        if (animalData) {
            m_resultsStream << animalData->state().position().x() << "\t";
            m_resultsStream << animalData->state().position().y() << "\t";
            m_resultsStream << animalData->state().orientation().angle() << "\t";
        } else {
            m_resultsStream << "NaN\t";
            m_resultsStream << "NaN\t";
            m_resultsStream << "NaN\t";
        }
    }

    m_resultsStream << endl;
}

/*!
 * Searches for the robot of the given type and corresponding to the given index
 * in the indexToId correspondence map, if the agent is not yet in the map but
 * there are free agents unassigned then one is taken and assigned;
 * the pointer to the agent is returned. If no agent is found then a nullptr
 * is returned.
  */
const AgentDataWorld* TrajectoryWriter::getAgentData(int index, AgentType agentType, QMap<int, QString>& indexToId, const QList<AgentDataWorld>& agentsData)
{
    if (indexToId.contains(index)) {
        // the agent is known
        QString id = indexToId[index];
        // search if it's provided
        for (int i = 0; i < agentsData.size(); i++)
            // if found then return its address
            if (agentsData[i].id() == id)
                return &agentsData[i];
        // if nothing found then return a null ptr
        return nullptr;
    } else {
        QList<QString> usedIds = indexToId.values();
        // this index is not yet used, try to find any agent to assign
        for (int i = 0; i < agentsData.size(); i++) {
            if ((agentsData[i].type() == agentType) && (!usedIds.contains(agentsData[i].id()))) {
                // found one agent that is not yet used
                indexToId.insert(index, agentsData[i].id());
                return &agentsData[i];
            }
        }
        // no agent found
        return nullptr;
    }
}
