#ifndef CATS2_TRAJECTORY_WRITER_HPP
#define CATS2_TRAJECTORY_WRITER_HPP

#include <AgentData.hpp>

#include <QtCore/QFile>
#include <QtCore/QTextStream>

/*!
 * \brief The class that writes the tracking results to the text file.
 */
class TrajectoryWriter
{
public:
    //! Constructor.
    explicit TrajectoryWriter(QString dataLoggingPath);
    //! Destructor.
    virtual ~TrajectoryWriter() final;

    //! Saves the tracking results to the ouptup file.
    //! timestamp is the number of milliseconds since 1970-01-01T00:00:00
    //! Universal Coordinated Time.
    void writeData(std::chrono::milliseconds timestamp,
                   const QList<AgentDataWorld>& agentsData);

private:
    //! Searches for the robot of the given type and corresponding to the given
    //! index in the indexToId correspondence map.
    const AgentDataWorld* getAgentData(int index,
                                       AgentType agentType,
                                       QMap<int, QString>& indexToId,
                                       const QList<AgentDataWorld>& agentsData);

private:
    //! The file to write the tracking results.
    QFile m_resultsFile;
    //! The output stream.
    QTextStream m_resultsStream;

    //! Maps robots' indices in the output file to the robots id.
    QMap<int, QString> m_robotsIndexToId;
    //! Maps fish' indices in the output file to the fish id.
    QMap<int, QString> m_fishIndexToId;
};

#endif // CATS2_TRAJECTORY_WRITER_HPP
