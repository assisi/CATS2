#ifndef CATS2_EXPERIMENT_CONTROLLER_TYPE_HPP
#define CATS2_EXPERIMENT_CONTROLLER_TYPE_HPP

#include <QtCore/QString>

/*!
 * Provides an enum for experiment controller type.
 */
class ExperimentControllerType
{
public:
    enum Enum {
        NONE,
        CONTROL_MAP,
        INITIATION
    };

    //! Gets the type of the experiment controller type from the settings'
    //! string.
    static Enum fromSettingsString(QString controllerName)
    {
        if (controllerName.toLower() == "controlmap")
            return CONTROL_MAP;
        if (controllerName.toLower() == "initiation")
            return INITIATION;
        else if (controllerName.toLower() == "none")
            return NONE;
        else
            return NONE;
    }

    //! Returns the controller type string that is used to look for the settings
    //! values in the configuration file.
    static QString toSettingsString(Enum controllerType) {
        QString string = "none";

        switch (controllerType) {
        case CONTROL_MAP:
            string = "controlMap";
            break;
        case INITIATION:
            string = "initiation";
            break;
        case NONE:
        default:
            break;
        }
        return string;
    }

    //! Returns that experiment controller type string in human friendly format.
    static QString toString(Enum controllerType) {
        QString string;

        switch (controllerType) {
        case CONTROL_MAP:
            string = "Control map";
            break;
        case INITIATION:
            string = "Initiation";
            break;
        case NONE:
        default:
            string = "None";
            break;
        }
        return string;
    }
};

#endif // CATS2_EXPERIMENT_CONTROLLER_TYPE_HPP

