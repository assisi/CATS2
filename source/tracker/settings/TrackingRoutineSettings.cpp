#include "TrackingRoutineSettings.hpp"

#include <TrackingSetup.hpp>
#include <ReadSettingsHelper.hpp>

/*!
 * Constructor.
 */
TrackingRoutineSettings::TrackingRoutineSettings(SetupType setupType) :
    m_trackingRoutineType(TrackingRoutineType::UNDEFINED)
{
    m_settingPathPrefix = TrackingSetup::setupSettingsNameByType(setupType);
}

/*!
 * Destructor.
 */
TrackingRoutineSettings::~TrackingRoutineSettings()
{

}
