#ifndef CATS2_TRACKING_FACTORY_HPP
#define CATS2_TRACKING_FACTORY_HPP

#include "routines/TrackingRoutineType.hpp"
#include "routines/TrackingRoutine.hpp"
#include "routines/BlobDetector.hpp"
#include "settings/TrackingRoutineSettings.hpp"
#include "settings/BlobDetectorSettings.hpp"
#include "settings/TrackingSettings.hpp"
#include <SetupType.hpp>

/*!
 * \brief The factory class that generates a tracking routine of given type.
 */
class TrackerFactory
{
public:
    static TrackingRoutineType trackingRoutineTypeByName(QString routineName)
    {
        if (routineName.toLower() == "blobdetector")
            return TrackingRoutineType::BLOB_DETECTOR;
        else
            return TrackingRoutineType::UNDEFINED;
    }

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
            default:
                break;
            }
        }
        return TrackingRoutinePtr();
    }

    /*!
     * Generates the tracking routine settings of given type.
     */
    static TrackingRoutineSettingsPtr createTrackingRoutineSettings(TrackingRoutineType trackingType, SetupType::Enum setupType)
    {
        switch (trackingType) {
        case TrackingRoutineType::BLOB_DETECTOR:
            return TrackingRoutineSettingsPtr(new BlobDetectorSettings(setupType));
        default:
            break;
        }
        return TrackingRoutineSettingsPtr();
    }

};

#endif // CATS2_TRACKING_FACTORY_HPP
