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
        DOMINATING_SET,
        CIRCULAR_SETUP_FOLLOWER,
        CIRCULAR_SETUP_LEADER_CW,
        CIRCULAR_SETUP_LEADER_CCW,
        CIRCULAR_SETUP_FOLLOWER_MODEL,
        CIRCULAR_SETUP_LEADER_CW_MODEL,
        CIRCULAR_SETUP_LEADER_CCW_MODEL,
        UNDEFINED
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
            string = "initiationLeader";
            break;
        case INITIATION_LURE:
            string = "initiationLure";
            break;
        case DOMINATING_SET:
            string = "domset";
            break;
        case CIRCULAR_SETUP_FOLLOWER:
        case CIRCULAR_SETUP_LEADER_CW:
        case CIRCULAR_SETUP_LEADER_CCW:
        case CIRCULAR_SETUP_FOLLOWER_MODEL:
        case CIRCULAR_SETUP_LEADER_CW_MODEL:
        case CIRCULAR_SETUP_LEADER_CCW_MODEL:
            string = "circularSetup";
            break;
        case NONE:
        case UNDEFINED:
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
        case CIRCULAR_SETUP_FOLLOWER:
            string = "Circular setup follower";
            break;
        case CIRCULAR_SETUP_LEADER_CW:
            string = "Circular setup leader (CW)";
            break;
        case CIRCULAR_SETUP_LEADER_CCW:
            string = "Circular setup leader (CCW)";
            break;
        case CIRCULAR_SETUP_FOLLOWER_MODEL:
            string = "Circular setup follower with model";
            break;
        case CIRCULAR_SETUP_LEADER_CW_MODEL:
            string = "Circular setup leader with model (CW)";
            break;
        case CIRCULAR_SETUP_LEADER_CCW_MODEL:
            string = "Circular setup leader with model (CCW)";
            break;
        case NONE:
            string = "Disabled";
            break;
        case UNDEFINED:
        default:
            string = "Undefined";
            break;
        }
        return string;
    }
};

#endif // CATS2_EXPERIMENT_CONTROLLER_TYPE_HPP

