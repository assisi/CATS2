#include "TrackingRoutineSettings.hpp"

#include <ReadSettingsHelper.hpp>

/*!
 * Constructor.
 */
TrackingRoutineSettings::TrackingRoutineSettings(QString settingPathPrefix) :
    _trackingRoutineType(TrackingRoutineType::UNDEFINED),
    _settingPathPrefix(settingPathPrefix)
{

}

/*!
 * Destructor.
 */
TrackingRoutineSettings::~TrackingRoutineSettings()
{

}
