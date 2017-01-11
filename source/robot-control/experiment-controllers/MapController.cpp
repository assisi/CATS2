#include "MapController.hpp"
#include "FishBot.hpp"
#include "ControlArea.hpp"

#include <settings/ReadSettingsHelper.hpp>

/*!
 * Constructor. Gets the file name containing the control map description.
 */
MapController::MapController(FishBot* robot, QString controlAreasFileName) :
    ExperimentController(robot, controlAreasFileName, ExperimentControllerType::CONTROL_MAP)
{

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
        if (isValid() && position.isValid()) {
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



