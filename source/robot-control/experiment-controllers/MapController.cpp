#include "MapController.hpp"
#include "FishBot.hpp"
#include "ControlArea.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
MapController::MapController(FishBot* robot,
                             ExperimentControllerSettingsPtr settings) :
    ExperimentController(robot, ExperimentControllerType::CONTROL_MAP),
    m_settings()
{
    // NOTE : to get parameters specific for this controller we need to convert
    // the settings to the corresponding format
    MapControllerSettings* mapControllerSettings =
            dynamic_cast<MapControllerSettings*>(settings.data());
    if (mapControllerSettings != nullptr){
        // copy the parameters
        m_settings = mapControllerSettings->data();
    } else {
        qDebug() << "Could not set the controller's settings";
    }

    // load the control map
    // if there is no robot specifi control map then take form the default settings
    if (!readRobotControlMap())
        readControlMap(m_settings.defaultControlAreasFileName());
}

/*!
 * Returns the control values for given position. If it's not in the map
 * then default (UNDEFINED) values are returned.
 */
ExperimentController::ControlData MapController::step()
{
    // values are initialized as undefined
    ControlData controlData;

    // check where the robot and fish are
    updateAreasOccupation();

    // based on where is the robot we update the command
    if (m_controlAreas.contains(m_robotAreaId)) {
        controlData.controlMode = m_controlAreas[m_robotAreaId]->controlMode();
        controlData.motionPattern = m_controlAreas[m_robotAreaId]->motionPattern();

        // if we are happen to be in the fish model control mode when we need
        // to limit the model to the current area
        if ((controlData.controlMode == ControlModeType::MODEL_BASED) &&
                m_robotAreaChanged)
        {
            controlData.data =
                QVariant::fromValue(m_controlAreas[m_robotAreaId]->annotatedPolygons());
        }
        return controlData;
    }

    return controlData;
}



