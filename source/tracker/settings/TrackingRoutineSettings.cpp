#include "TrackingRoutineSettings.hpp"
#include "TrackingSetup.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
TrackingRoutineSettings::TrackingRoutineSettings(SetupType::Enum setupType) :
    m_trackingRoutineType(TrackingRoutineType::UNDEFINED)
{
    m_settingPathPrefix = SetupType::toSettingsString(setupType);
}

/*!
 * Destructor.
 */
TrackingRoutineSettings::~TrackingRoutineSettings()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}
