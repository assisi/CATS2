#include "TrackingRoutineSettings.hpp"

#include <TrackingSetup.hpp>
#include <ReadSettingsHelper.hpp>

/*!
 * Constructor.
 */
TrackingRoutineSettings::TrackingRoutineSettings(SetupType setupType) :
    _trackingRoutineType(TrackingRoutineType::UNDEFINED)
{
    _settingPathPrefix = TrackingSetup::setupSettingsNameByType(setupType);
}

/*!
 * Destructor.
 */
TrackingRoutineSettings::~TrackingRoutineSettings()
{

}
