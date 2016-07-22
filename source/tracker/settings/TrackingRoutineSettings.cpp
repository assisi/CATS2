#include "TrackingRoutineSettings.hpp"

#include <TrackingSetup.hpp>
#include <settings/ReadSettingsHelper.hpp>

/*!
 * Constructor.
 */
TrackingRoutineSettings::TrackingRoutineSettings(SetupType::Enum setupType) :
    m_trackingRoutineType(TrackingRoutineType::UNDEFINED)
{
    m_settingPathPrefix = SetupType::toString(setupType);
}

/*!
 * Destructor.
 */
TrackingRoutineSettings::~TrackingRoutineSettings()
{

}
