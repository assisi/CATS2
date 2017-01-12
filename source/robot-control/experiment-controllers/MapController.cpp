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
        qDebug() << Q_FUNC_INFO << "Could not set the controller's settings";
    }

    // load the control map
    readControlMap(m_settings.controlAreasFileName());
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
        return controlData;
    }

    return controlData;
}



