#ifndef CATS2_EXPERIMENT_CONTROLLER_FACTORY_HPP
#define CATS2_EXPERIMENT_CONTROLLER_FACTORY_HPP

#include "RobotControlPointerTypes.hpp"
#include "experiment-controllers/ExperimentControllerType.hpp"
#include "experiment-controllers/MapController.hpp"
#include "experiment-controllers/InitiationLeaderController.hpp"
#include "experiment-controllers/InitiationLureController.hpp"
#include "settings/MapControllerSettings.hpp"
#include "settings/RobotControlSettings.hpp"
#include "settings/InitiationLureControllerSettings.hpp"
#include "settings/InitiationLeaderControllerSettings.hpp"

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
            switch (settings->type()) {
            case ExperimentControllerType::CONTROL_MAP:
                return ExperimentControllerPtr(new MapController(robot, settings), &QObject::deleteLater);
            case ExperimentControllerType::INITIATION_LEADER:
                return ExperimentControllerPtr(new InitiationLeaderController(robot, settings), &QObject::deleteLater);
            case ExperimentControllerType::INITIATION_LURE:
                return ExperimentControllerPtr(new InitiationLureController(robot, settings), &QObject::deleteLater);
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
        default:
            qDebug() << "Experiment controller settings could not be created.";
            break;
        }
        return ExperimentControllerSettingsPtr();
    }
};

#endif // CATS2_EXPERIMENT_CONTROLLER_FACTORY_HPP

