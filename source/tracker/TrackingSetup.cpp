#include "TrackingSetup.hpp"

#include <TimestampedFrame.hpp>
#include <CoordinatesConversion.hpp>
#include <settings/CalibrationSettings.hpp>
#include <GrabberHandler.hpp>

/*!
 * Constructor.
 */
TrackingSetup::TrackingSetup(SetupType::Enum setupType) :
    m_setupType(setupType),
    m_coordinatesConversion(new CoordinatesConversion(CalibrationSettings::get().calibrationFilePath(setupType))),
    m_grabber(new GrabberHandler(setupType)),
    m_tracking(new TrackingHandler(setupType, m_coordinatesConversion, m_grabber->inputQueue()))
{

}
