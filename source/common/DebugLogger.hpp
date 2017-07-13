#ifndef CATS2_DEBUG_LOGGER_HPP
#define CATS2_DEBUG_LOGGER_HPP

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QMutex>

/*!
 * Redirects the debug output to a text file.
 */
class DebugLogger
{
public:
    //! Constructor. Defining it here prevents construction.
    explicit DebugLogger();
    //! Destructor. Defining it here prevents unwanted destruction.
    ~DebugLogger();

    //! Saves the current time as the program start time.
    void init();

public:
    //! Writes the log message.
    void messageOutput(QtMsgType type,
                       const QMessageLogContext &context,
                       const QString &msg);

private:
    //! The file to write the log.
    QFile m_logFile;
    //! The output stream.
    QTextStream m_logStream;
    //! The mutex to protect the data in a multi-thread applicatoin.
    QMutex m_mutex; // TODO : to check if it's necessary
};

#endif // CATS2_DEBUG_LOGGER_HPP

