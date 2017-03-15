#ifndef CATS2_TRACKING_FACTORY_HPP
#define CATS2_TRACKING_FACTORY_HPP

#include "routines/TrackingRoutineType.hpp"
#include "routines/TrackingRoutine.hpp"
#include "routines/BlobDetector.hpp"
#include "routines/ColorDetector.hpp"
#include "routines/FishBotLedsTracking.hpp"
#include "routines/TwoColorsTagTracking.hpp"
#include "settings/TrackingRoutineSettings.hpp"
#include "settings/BlobDetectorSettings.hpp"
#include "settings/FishBotLedsTrackingSettings.hpp"
#include "settings/ColorDetectorSettings.hpp"
#include "settings/TwoColorsTagTrackingSettings.hpp"
#include "settings/TrackingSettings.hpp"
#include <SetupType.hpp>

/*!
 * \brief The factory class that generates a tracking routine of given type.
 */
class TrackerFactory
{
public:
    /*!
     * Generates the tracking routine of given type.
     */
    static TrackingRoutinePtr createTrackingRoutine(SetupType::Enum setupType, TimestampedFrameQueuePtr inputQueue, TimestampedFrameQueuePtr debugQueue)
    {
        TrackingRoutineSettingsPtr settings = TrackingSettings::get().trackingRoutineSettings(setupType);

        if (!settings.isNull()) {
            switch (settings->type()) {
            case TrackingRoutineType::BLOB_DETECTOR:
                return TrackingRoutinePtr(new BlobDetector(settings, inputQueue, debugQueue), &QObject::deleteLater);
            case TrackingRoutineType::COLOR_DETECTOR:
                return TrackingRoutinePtr(new ColorDetector(settings, inputQueue, debugQueue), &QObject::deleteLater);
            case TrackingRoutineType::FISHBOT_LEDS_TRACKING:
                return TrackingRoutinePtr(new FishBotLedsTracking(settings, inputQueue, debugQueue), &QObject::deleteLater);
            case TrackingRoutineType::TWO_COLORS_TAG_TRACKING:
                return TrackingRoutinePtr(new TwoColorsTagTracking(settings, inputQueue, debugQueue), &QObject::deleteLater);
            default:
                qDebug() << Q_FUNC_INFO << "Tracking routine could not be created.";
                break;
            }
        }
        return TrackingRoutinePtr();
    }

    /*!
     * Generates the tracking routine settings of given type.
     */
    static TrackingRoutineSettingsPtr createTrackingRoutineSettings(TrackingRoutineType::Enum trackingType, SetupType::Enum setupType)
    {
        switch (trackingType) {
        case TrackingRoutineType::BLOB_DETECTOR:
            return TrackingRoutineSettingsPtr(new BlobDetectorSettings(setupType));
        case TrackingRoutineType::COLOR_DETECTOR:
            return TrackingRoutineSettingsPtr(new ColorDetectorSettings(setupType));
        case TrackingRoutineType::FISHBOT_LEDS_TRACKING:
            return TrackingRoutineSettingsPtr(new FishBotLedsTrackingSettings(setupType));
        case TrackingRoutineType::TWO_COLORS_TAG_TRACKING:
            return TrackingRoutineSettingsPtr(new TwoColorsTagTrackingSettings(setupType));
        default:
            qDebug() << Q_FUNC_INFO << "Tracking routine settings could not be created.";
            break;
        }
        return TrackingRoutineSettingsPtr();
    }

};

#endif // CATS2_TRACKING_FACTORY_HPP
