#ifndef CATS2_CONTROL_MODE_TYPE_HPP
#define CATS2_CONTROL_MODE_TYPE_HPP

#include <QtCore/QString>

/*!
 * Provides an enum for control mode type.
 */
class ControlModeType
{
public:
    enum Enum {
        IDLE,
        MANUAL,
        GO_TO_POSITION,
        GO_STRAIGHT
    };

    //! Returns that control mode type string in human friendly format.
    static QString toString(Enum controlModeType) {
        QString string;

        switch (controlModeType) {
        case MANUAL:
            string = "Manual";
            break;
        case GO_TO_POSITION:
            string = "Go to position";
            break;
        case GO_STRAIGHT:
            string = "Go straight";
            break;
        case IDLE:
        default:
            string = "Idle";
            break;
        }
        return string;
    }
};

#endif // CATS2_CONTROL_MODE_TYPE_HPP

