#ifndef CATS2_TRACKING_FACTORY_HPP
#define CATS2_TRACKING_FACTORY_HPP

#include "routines/TrackingRoutineType.hpp"
#include "routines/TrackingRoutine.hpp"
#include "routines/BlobDetector.hpp"
#include "settings/TrackingRoutineSettings.hpp"
#include "settings/BlobDetectorSettings.hpp"
#include "settings/TrackingSettings.hpp"

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
    static TrackingRoutinePtr createTrackingRoutine(SetupType setupType, TimestampedFrameQueuePtr inputQueue, TimestampedFrameQueuePtr debugQueue)
    {
        TrackingRoutineSettingsPtr settings = TrackingSettings::get().trackingRoutineSettings(setupType);

        if (!settings.isNull()) {
            switch (settings.data()->type()) {
            case TrackingRoutineType::BLOB_DETECTOR:
                return TrackingRoutinePtr(new BlobDetector(settings, inputQueue, debugQueue), &QObject::deleteLater); // delete later is used for security as multithreaded
                                                                                                            // signals and slots might result is crashes when a
                                                                                                            // a sender is deleted before a signal is received for instance
            default:
                break;
            }
        }
        return TrackingRoutinePtr();
    }

    /*!
     * Generates the tracking routine settings of given type.
     */
    static QSharedPointer<TrackingRoutineSettings> createTrackingRoutineSettings(TrackingRoutineType trackingType, SetupType setupType)
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
