#ifndef CATS2_TRACKING_UI_FACTORY_HPP
#define CATS2_TRACKING_UI_FACTORY_HPP

#include "TrackerPointerTypes.hpp"

#include <QtWidgets/QWidget>

/*!
 * This class generates the tracking settings widget corresponding to the
 * routine used by the tracking data class.
 */
class TrackingUiFactory
{
public:
    //! Generates the tracking routine of given type.
    static QWidget* createWidget(TrackingDataPtr trackingData);
};


#endif // CATS2_TRACKING_UI_FACTORY_HPP

