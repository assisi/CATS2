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
        INITIATION_LEADER,
        INITIATION_LURE,
        DOMINATING_SET
    };

    //! Returns the controller type string that is used to look for the settings
    //! values in the configuration file.
    static QString toSettingsString(Enum controllerType) {
        QString string = "none";

        switch (controllerType) {
        case CONTROL_MAP:
            string = "controlMap";
            break;
        case INITIATION_LEADER:
        case INITIATION_LURE:
            string = "initiation";
            break;
        case DOMINATING_SET:
            string = "domset";
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
        case INITIATION_LEADER:
            string = "Initiation leader";
            break;
        case INITIATION_LURE:
            string = "Initiation lure";
            break;
        case DOMINATING_SET:
            string = "Dominating set";
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

