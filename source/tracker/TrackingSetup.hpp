#ifndef CATS2_TRACKING_SETUP_HPP
#define CATS2_TRACKING_SETUP_HPP

#include "TrackingHandler.hpp"
#include "TrackerPointerTypes.hpp"

#include <CommonPointerTypes.hpp>
#include <GrabberPointerTypes.hpp>
#include <HubPointerTypes.hpp>

#include <gui/TrackingRoutineWidget.hpp>

#include <QtCore/QSharedPointer>
#include <QtCore/QMap>

/*!
 * \brief The TrackingSetup class corresponds to a arena tracking procedures, it brings together
 * a video grabber attached to a tracker and sends out the detected agents' positions converted to
 * world coordinates by using a corresponding coordinates transformation.
 */
class TrackingSetup
{
public:
    //! Constructor. The needOutputQueue specifies if this class needs to provide
    //! an queue with the camera images to be shown on an external GUI.
    TrackingSetup(SetupType::Enum setupType, bool needOutputQueue = false);

    //! Destructor.
    virtual ~TrackingSetup() final;

public:
    //! Returns the pointer to the tracking routine gui.
    QWidget* trackingWidget() { return m_tracking->widget(); }

    //! Returns the pointer to the hub's output queue.
    TimestampedFrameQueuePtr viewerQueue();

    //! Returns the pointer to the coordinates transformation.
    CoordinatesConversionPtr coordinatesConversion() { return m_coordinatesConversion; }

private:
    //! The type of setup, for instance, main camera or the camera below.
    SetupType::Enum m_setupType;

    //! Input queue for the grabber.
    TimestampedFrameQueuePtr m_queue;
    //! The coordinate conversion to trasform the agents' position in pixels to positions in meters.
    CoordinatesConversionPtr m_coordinatesConversion;
    //! The grabber that receives the video stream.
    GrabberHandlerPtr m_grabber;
    //! The tracker that detects and tracks agents in the income stream.
    TrackingHandlerPtr m_tracking;

    //! The queue hub to duplicate the frames from the input queue to several output queues. It's created only when
    //! it's requested with a specific flag in the constructor.
    QueueHubPtr m_queueHub;
};

#endif // CATS2_TRACKING_SETUP_HPP

