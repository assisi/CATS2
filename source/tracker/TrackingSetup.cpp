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
    m_coordinatesConversion(new CoordinatesConversion(CalibrationSettings::get().calibrationFilePath(setupType),
                                                      CalibrationSettings::get().frameSize(setupType))),
    m_grabber(new GrabberHandler(setupType))
{
    // if there is no need to expose camera images in the additional queue then the tracker gets directly the images
    // from the grabber
    if (!needOutputQueue) {
        m_tracking = TrackingHandlerPtr(new TrackingHandler(setupType, m_coordinatesConversion, m_grabber->inputQueue()));
    } else {
        // otherwise we need to introduce the multiplicator that will take care about the extra queue
        m_queueHub = QueueHubPtr(new QueueHub(m_grabber->inputQueue()));
        m_tracking = TrackingHandlerPtr(new TrackingHandler(setupType, m_coordinatesConversion, m_queueHub->addOutputQueue()));
    }
}

/*!
 * Returns the pointer to the hub's output queue.
 */
TimestampedFrameQueuePtr TrackingSetup::viewerQueue()
{
    if (!m_queueHub.isNull())
        return m_queueHub->addOutputQueue();
    else
        return TimestampedFrameQueuePtr();
}
