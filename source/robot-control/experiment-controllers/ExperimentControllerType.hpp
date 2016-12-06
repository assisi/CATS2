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
        CONTROL_MAP
    };

    //! Gets the type of the experiment controller type from the settings' string.
    static Enum fromSettingsString(QString controllerName)
    {
        if (controllerName.toLower() == "controlmap")
            return CONTROL_MAP;
        else if (controllerName.toLower() == "none")
            return NONE;
        else
            return NONE;
    }

    //! Returns that experiment controller type string in human friendly format.
    static QString toString(Enum controllerType) {
        QString string;

        switch (controllerType) {
        case CONTROL_MAP:
            string = "Control map";
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

