#include "DebugLogger.hpp"

#include <QtCore/QDebug>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtWidgets/QApplication>
#include <QtCore/QMutexLocker>
#include <QtCore/QDateTime>

#include <iostream>

/*!
 * Constructor.
 */
DebugLogger::DebugLogger()
{

}

/*!
 * Destructor.
 */
DebugLogger::~DebugLogger()
{
    m_logStream << "Logging finished" << endl;
    m_logFile.close();
    qDebug() << Q_FUNC_INFO << "Destroying the object" << endl;
}

/*!
 * Opens the log file.
 */
void DebugLogger::init()
{
    // create the output folder if necessary
    QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))
            .mkdir(QApplication::applicationName());
    QString loggingPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
            QDir::separator() + QApplication::applicationName();
    // create the folder for debug logs
    QDir(loggingPath).mkdir("logs");
    loggingPath = loggingPath + QDir::separator() + "logs";
    // form the file name
    QString fileName = QString("log-%2.txt")
            .arg(QDateTime::currentDateTime().toString("yyyy.MM.dd-hh:mm:ss"));
    // open the text where to write the tracking results
    m_logFile.setFileName(loggingPath + QDir::separator() + fileName);
    if (m_logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_logStream.setDevice(&m_logFile);
        // write the header
        m_logStream << "Logging started" << endl;
    }
}

/*!
 * Writes the log message.
 */
void DebugLogger::messageOutput(QtMsgType type,
                                const QMessageLogContext &context,
                                const QString &msg)
{
    QMutexLocker locker(&m_mutex);
    QByteArray localMsg = msg.toLocal8Bit();
    m_logStream << localMsg.constData() << endl;
    // duplicate in the console
    std::cout << localMsg.constData() << std::endl;
}
