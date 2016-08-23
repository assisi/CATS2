#include "TrackingSetup.hpp"

#include <TimestampedFrame.hpp>
#include <CoordinatesConversion.hpp>
#include <settings/CalibrationSettings.hpp>
#include <GrabberHandler.hpp>
#include <QueueHub.hpp>

/*!
 * Constructor.
 */
TrackingSetup::TrackingSetup(SetupType::Enum setupType, bool needOutputQueue) :
    m_setupType(setupType),
    m_grabber(new GrabberHandler(setupType)),
    m_tracking(new TrackingHandler(setupType, m_coordinatesConversion, m_grabber->inputQueue()))
    m_coordinatesConversion(new CoordinatesConversion(CalibrationSettings::get().calibrationFilePath(setupType),
                                                      CalibrationSettings::get().frameSize(setupType)))
{
}
