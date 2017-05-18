#include "DominatingSetController.hpp"

#include "FishBot.hpp"
#include "ControlArea.hpp"

#include <QtCore/QDebug>

DominatingSetController::DominatingSetController(FishBot* robot,
                                                ExperimentControllerSettingsPtr settings) :
    ExperimentController(robot, ExperimentControllerType::DOMINATING_SET),
    m_settings()
{
    // NOTE : to get parameters specific for this controller we need to convert
    // the settings to the corresponding format
    DominatingSetControllerSettings* controllerSettings =
            dynamic_cast<DominatingSetControllerSettings*>(settings.data());
    if (controllerSettings != nullptr){
        // copy the parameters
        m_settings = controllerSettings->data();
    } else {
        qDebug() << "Could not set the controller's settings";
    }

    // load the control map
    readControlMap(m_settings.controlAreasFileName());
}

/*!
 * Returns the control values for given position.
 */
ExperimentController::ControlData DominatingSetController::step()
{
    ControlData controlData;
    // a check for the valid robot pointer
    if (m_robot) {
        // check where the robot and fish are
        updateAreasOccupation();

        // TODO : to implement
    }
    return controlData;
}
