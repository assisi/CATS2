#ifndef CATS2_SETUP_TYPE_HPP
#define CATS2_SETUP_TYPE_HPP

#include <QMap>

/*!
 * Provides an enum for various setup types and toString method.
 */
class SetupType
{
public:
    enum Enum {
        MAIN_CAMERA, // camera fixed above the setup
        CAMERA_BELOW // camera below the robot's support
    };

    static QString toString(Enum setupType) {
        QString string = "undefined";

        switch (setupType) {
        case MAIN_CAMERA:
            string = "mainCamera";
            break;
        case CAMERA_BELOW:
            string = "cameraBelow";
            break;
        default:
            break;
        }
        return string;
    }
};

#endif // CATS2_SETUP_TYPE_HPP
