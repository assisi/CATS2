#ifndef CATS2_TRACKING_ROUTINE_TYPE_HPP
#define CATS2_TRACKING_ROUTINE_TYPE_HPP

#include <QtCore/QString>

/*!
 * \brief All types of tracking routines that can be used.
 */
class TrackingRoutineType
{
public:
    enum Enum {
        COLOR_DETECTOR,
        BLOB_DETECTOR,
        FISHBOT_LEDS_TRACKING,
        TWO_COLORS_TAG_TRACKING,
        // to extend
        UNDEFINED
    };

    //! Gets the type of the tracking routine from the settings' string.
    static Enum fromSettingsString(QString routineName)
    {
        if (routineName.toLower() == "blobdetector")
            return TrackingRoutineType::BLOB_DETECTOR;
        else if (routineName.toLower() == "fishbotledstracking")
            return TrackingRoutineType::FISHBOT_LEDS_TRACKING;
        else if (routineName.toLower() == "colordetector")
            return TrackingRoutineType::COLOR_DETECTOR;
        else if (routineName.toLower() == "twocolorstagtracking")
            return TrackingRoutineType::TWO_COLORS_TAG_TRACKING;
        else
            return TrackingRoutineType::UNDEFINED;
    }

    //! Returns the tracking routine type string in human friendly format.
    static QString toString(Enum setupType) {
        QString string = "Undefined";

        switch (setupType) {
        case COLOR_DETECTOR:
            string = "Color spots detection";
            break;
        case BLOB_DETECTOR:
            string = "Basic blob detection";
            break;
        case FISHBOT_LEDS_TRACKING:
            string = "FishBot LEDs tracker";
            break;
        case TWO_COLORS_TAG_TRACKING:
            string = "Two colors tag tracker";
            break;
        default:
            break;
        }
        return string;
    }
};

#endif // CATS2_TRACKING_ROUTINE_TYPE_HPP
