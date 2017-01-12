#ifndef CATS2_EXPERIMENT_CONTROLLER_FACTORY_HPP
#define CATS2_EXPERIMENT_CONTROLLER_FACTORY_HPP

#include "RobotControlPointerTypes.hpp"
#include "experiment-controllers/ExperimentControllerType.hpp"
#include "experiment-controllers/MapController.hpp"
#include "experiment-controllers/InitiationController.hpp"
#include "settings/MapControllerSettings.hpp"
#include "settings/RobotControlSettings.hpp"
#include "settings/InitiationControllerSettings.hpp"

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
            case ExperimentControllerType::INITIATION:
                return ExperimentControllerPtr(new InitiationController(robot, settings), &QObject::deleteLater);
            default:
                qDebug() << Q_FUNC_INFO << "Controller could not be created.";
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
        case ExperimentControllerType::INITIATION:
            return ExperimentControllerSettingsPtr(new InitiationControllerSettings());
        default:
            qDebug() << Q_FUNC_INFO
                     << "Experiment controller settings could not be created.";
            break;
        }
        return ExperimentControllerSettingsPtr();
    }
};

#endif // CATS2_EXPERIMENT_CONTROLLER_FACTORY_HPP

