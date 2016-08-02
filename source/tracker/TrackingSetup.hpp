#ifndef CATS2_TRACKING_SETUP_HPP
#define CATS2_TRACKING_SETUP_HPP

#include "TrackingHandler.hpp"

#include <CommonPointerTypes.hpp>
#include <GrabberData.hpp>

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
    //! Constructor.
    TrackingSetup(SetupType::Enum type);

private:
    //! The type of setup, for instance, main camera or the camera below.
    SetupType::Enum m_type;

    //! The grabber.
    QSharedPointer<GrabberData> m_grabber;
    //! The tracker.
    QSharedPointer<TrackingHandler> m_tracking;


    // TODO : add Grabber / Viewer / Tracker / CoordinateTransformation instance here, the are to be initialized from the settings based on the given setup type
};

#endif // CATS2_TRACKING_SETUP_HPP

