#include "TrackingUiFactory.hpp"

#include "TrackingData.hpp"
#include "routines/BlobDetector.hpp"
#include "gui/BlobDetectorWidget.hpp"
#include "gui/ColorDetectorWidget.hpp"
#include "gui/TwoColorsTagsTrackingWidget.hpp"
#include "gui/FishBotLedsTrackingWidget.hpp"

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
            break;
        }
        case TrackingRoutineType::COLOR_DETECTOR:
        {
            widget = new ColorDetectorWidget(trackingData->m_trackingRoutine);
            break;
        }
        case TrackingRoutineType::TWO_COLORS_TAG_TRACKING:
        {
            widget = new TwoColorsTagsTrackingWidget(trackingData->m_trackingRoutine);
            break;
        }
        case TrackingRoutineType::FISHBOT_LEDS_TRACKING:
        {
            widget = new FishBotLedsTrackingWidget(trackingData->m_trackingRoutine);
            break;
        }
        default:
            // TODO : to implement
            qDebug() << "Tracking routine settings widget could not be created.";
            break;
    }
    return widget;
}
