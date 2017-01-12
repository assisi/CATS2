#include "MapController.hpp"
#include "FishBot.hpp"
#include "ControlArea.hpp"

#include <settings/ReadSettingsHelper.hpp>

/*!
 * Constructor. Gets the file name containing the control map description.
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

    if (m_robot) {
        PositionMeters position = m_robot->state().position();
        if (position.isValid()) {
            QPointF point(position.x(), position.y());
            for (const auto controlArea : m_controlAreas.values()) {
                if (controlArea->contains(point)) {
                    controlData.controlMode = controlArea->controlMode();
                    controlData.motionPattern = controlArea->motionPattern();
                    return controlData;
                }
            }
        }

    }

    return controlData;
}



