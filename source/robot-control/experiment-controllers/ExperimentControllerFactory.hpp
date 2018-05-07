#ifndef CATS2_EXPERIMENT_CONTROLLER_FACTORY_HPP
#define CATS2_EXPERIMENT_CONTROLLER_FACTORY_HPP

#include "RobotControlPointerTypes.hpp"
#include "experiment-controllers/ExperimentControllerType.hpp"
#include "experiment-controllers/MapController.hpp"
#include "experiment-controllers/InitiationLeaderController.hpp"
#include "experiment-controllers/InitiationLureController.hpp"
#include "experiment-controllers/DominatingSetController.hpp"
#include "experiment-controllers/CircularSetupFollowerController.hpp"
#include "experiment-controllers/CircularSetupLeaderController.hpp"
#include "experiment-controllers/CircularSetupLeaderWithModelController.hpp"
#include "settings/MapControllerSettings.hpp"
#include "settings/RobotControlSettings.hpp"
#include "settings/InitiationLureControllerSettings.hpp"
#include "settings/InitiationLeaderControllerSettings.hpp"
#include "settings/DominatingSetControllerSettings.hpp"
#include "settings/CircularSetupControllerSettings.hpp"

#include <QtCore/QDebug>

/*!
 * \brief The factory class that generates settings for every controller type.
 */
class ExperimentControllerFactory
{
public:
    /*!
     * Generates the controller of given type.
     */
    static ExperimentControllerPtr createController(ExperimentControllerType::Enum type,
                                                    FishBot* robot)
    {
        ExperimentControllerSettingsPtr settings =
                RobotControlSettings::get().controllerSettings(type);

        if (!settings.isNull()) {
            switch (type) {
            case ExperimentControllerType::CONTROL_MAP:
                return ExperimentControllerPtr(new MapController(robot, settings), &QObject::deleteLater);
            case ExperimentControllerType::INITIATION_LEADER:
                return ExperimentControllerPtr(new InitiationLeaderController(robot, settings), &QObject::deleteLater);
            case ExperimentControllerType::INITIATION_LURE:
                return ExperimentControllerPtr(new InitiationLureController(robot, settings), &QObject::deleteLater);
            case ExperimentControllerType::DOMINATING_SET:
                return ExperimentControllerPtr(new DominatingSetController(robot, settings), &QObject::deleteLater);
            case ExperimentControllerType::CIRCULAR_SETUP_FOLLOWER:
                return ExperimentControllerPtr(new CircularSetupFollowerController(robot, settings), &QObject::deleteLater);
            case ExperimentControllerType::CIRCULAR_SETUP_LEADER_CW:
                return ExperimentControllerPtr(new CircularSetupLeaderController(robot, settings, TurningDirection::CLOCK_WISE), &QObject::deleteLater);
            case ExperimentControllerType::CIRCULAR_SETUP_LEADER_CCW:
                return ExperimentControllerPtr(new CircularSetupLeaderController(robot, settings, TurningDirection::COUNTER_CLOCK_WISE), &QObject::deleteLater);
            case ExperimentControllerType::CIRCULAR_SETUP_LEADER_CW_MODEL:
                return ExperimentControllerPtr(new CircularSetupLeaderWithModelController(robot, settings, TurningDirection::CLOCK_WISE), &QObject::deleteLater);
            case ExperimentControllerType::CIRCULAR_SETUP_LEADER_CCW_MODEL:
                return ExperimentControllerPtr(new CircularSetupLeaderWithModelController(robot, settings, TurningDirection::COUNTER_CLOCK_WISE), &QObject::deleteLater);
            default:
                qDebug() << "Controller could not be created.";
                break;
            }
        }
        return ExperimentControllerPtr();
    }

    /*!
     * Generates the controller settings of given type.
     */
    static ExperimentControllerSettingsPtr createSettings(ExperimentControllerType::Enum type)
    {
        switch (type) {
        case ExperimentControllerType::CONTROL_MAP:
            return ExperimentControllerSettingsPtr(new MapControllerSettings());
        case ExperimentControllerType::INITIATION_LEADER:
            return ExperimentControllerSettingsPtr(new InitiationLeaderControllerSettings());
        case ExperimentControllerType::INITIATION_LURE:
            return ExperimentControllerSettingsPtr(new InitiationLureControllerSettings());
        case ExperimentControllerType::DOMINATING_SET:
            return ExperimentControllerSettingsPtr(new DominatingSetControllerSettings());
        case ExperimentControllerType::CIRCULAR_SETUP_FOLLOWER:
        case ExperimentControllerType::CIRCULAR_SETUP_LEADER_CW:
        case ExperimentControllerType::CIRCULAR_SETUP_LEADER_CCW:
        case ExperimentControllerType::CIRCULAR_SETUP_LEADER_CW_MODEL:
        case ExperimentControllerType::CIRCULAR_SETUP_LEADER_CCW_MODEL:
            // NOTE: two controllers share the same settings
            return ExperimentControllerSettingsPtr(new CircularSetupControllerSettings(type));
        default:
            qDebug() << "Experiment controller settings could not be created.";
            break;
        }
        return ExperimentControllerSettingsPtr();
    }
};

#endif // CATS2_EXPERIMENT_CONTROLLER_FACTORY_HPP

