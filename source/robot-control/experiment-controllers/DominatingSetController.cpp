#include "DominatingSetController.hpp"

#include "FishBot.hpp"
#include "ControlArea.hpp"

#include <QtCore/QDebug>

DominatingSetController::DominatingSetController(FishBot* robot,
                                                ExperimentControllerSettingsPtr settings) :
    ExperimentController(robot, ExperimentControllerType::DOMINATING_SET),
    m_settings(),
    m_robotAreaDefined(false)
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
 * Called when the controller is activated. Used to reset parameters.
 */
void DominatingSetController::start()
{
    m_robotAreaDefined = false;
}

/*!
 * Returns the control values for given position.
 */
ExperimentController::ControlData DominatingSetController::step()
{
    ControlData controlData;
    // a check for the valid robot pointer
    if (m_robot) {
        if (!m_robotAreaDefined) {
            updateAreasOccupation();
            // if the robot's zone is defined then set it in the model
            if (m_controlAreas.contains(m_robotAreaId)) {
                m_robotAreaDefined = true;
                controlData.controlMode = ControlModeType::MODEL_BASED;
                controlData.motionPattern = MotionPatternType::PID;
                controlData.data = QVariant::fromValue(m_controlAreas[m_robotAreaId]->annotatedPolygons());
            } else {
                // stop until it's detected
                controlData.controlMode = ControlModeType::IDLE;
            }
        } else {
            // do model motion
            controlData.controlMode = ControlModeType::MODEL_BASED;
            controlData.motionPattern = MotionPatternType::PID;
        }
    }
    return controlData;
}
