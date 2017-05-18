#include "CircularSetupController.hpp"

#include <QtCore/QDebug>

CircularSetupController::CircularSetupController(FishBot* robot,
                                                 ExperimentControllerType::Enum type,
                                                 ExperimentControllerSettingsPtr settings):
    ExperimentController(robot, type),
    m_settings()
{
    CircularSetupControllerSettings* controllerSettings =
            dynamic_cast<CircularSetupControllerSettings*>(settings.data());
    if (controllerSettings != nullptr){
        // copy the parameters
        m_settings = controllerSettings->data();
    } else {
        qDebug() << "Could not set the controller's settings";
    }
}

