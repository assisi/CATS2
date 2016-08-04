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

    //! Returns that setup type string that is used to look for settings values
    //! in the configuration file.
    static QString toSettingsString(Enum setupType) {
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

    //! Returns that setup type string in human friendly format.
    static QString toString(Enum setupType) {
        QString string = "Undefined";

        switch (setupType) {
        case MAIN_CAMERA:
            string = "Main camera";
            break;
        case CAMERA_BELOW:
            string = "Camera below";
            break;
        default:
            break;
        }
        return string;
    }
};

#endif // CATS2_SETUP_TYPE_HPP
