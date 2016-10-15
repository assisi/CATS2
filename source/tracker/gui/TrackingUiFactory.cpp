#include "TrackingUiFactory.hpp"

#include "TrackingData.hpp"
#include "routines/BlobDetector.hpp"
#include "gui/BlobDetectorWidget.hpp"

#include <QtCore/QDebug>

/*!
 * Generates the tracking routine of given type.
 */
QWidget* TrackingUiFactory::createWidget(TrackingDataPtr trackingData)
{
    QWidget* widget = nullptr;
    switch (trackingData->trackingType()) {
        case TrackingRoutineType::BLOB_DETECTOR:
        {
            widget = new BlobDetectorWidget(trackingData->m_trackingRoutine);
        }
        case TrackingRoutineType::FISHBOT_LEDS_TRACKING:
        default:
            // TODO : to implement
            qDebug() << Q_FUNC_INFO << "Tracking routine settings widget could not be created.";
            break;
    }
    return widget;
}
