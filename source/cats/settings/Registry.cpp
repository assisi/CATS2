#include "Registry.hpp"

#include <QtCore/QSettings>
#include <QtCore/QDebug>

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
     if (m_dataLoggingPath.isEmpty())
         qDebug() << Q_FUNC_INFO << "The logging path is not defined";
}

/*!
 * Updates the data loggins path.
 */
void Registry::setDataLoggingPath(QString path)
{
    QSettings settings;
    settings.setValue("dataLoggingPath", path);
}
