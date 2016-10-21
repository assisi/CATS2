#ifndef CATS2_MOTION_PATTERN_TYPE_HPP
#define CATS2_MOTION_PATTERN_TYPE_HPP

#include <QtCore/QString>

/*!
 * Provides an enum for motion pattern type.
 */
class MotionPatternType
{
public:
public:
    enum Enum {
        PID,
        FISH_MOTION,
        UNDEFINED
    };

    //! Returns that motion pattern type string in a human friendly format.
    static QString toString(Enum type) {
        QString string;

        switch (type) {
        case PID:
            string = "PID controller";
            break;
        default:
        case FISH_MOTION:
            string = "Fish motion";
            break;
        }
        return string;
    }
};

#endif // CATS2_MOTION_PATTERN_TYPE_HPP
