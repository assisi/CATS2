#include "Registry.hpp"

#include <QtCore/QSettings>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

#include <QtWidgets/QApplication>

/*!
 * The singleton getter. Provides an instance of the settings.
 */
Registry& Registry::get()
{
    static Registry instance;
    return instance;
}

/*!
 * Constructor.
 */
Registry::Registry() : QObject(nullptr)
{
     QSettings settings;
     m_dataLoggingPath = settings.value("dataLoggingPath", "").toString();
     if (m_dataLoggingPath.isEmpty()) {
         m_dataLoggingPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
                 QDir::separator() + QApplication::applicationName();
         qDebug() << Q_FUNC_INFO << QString("The logging path is not defined, "
                                              "setting the default path %1")
                                             .arg(m_dataLoggingPath);
     }
}

/*!
 * Updates the data loggins path.
 */
void Registry::setDataLoggingPath(QString path)
{
    QSettings settings;
    m_dataLoggingPath = path;
    settings.setValue("dataLoggingPath", path);
}
