#include "MapController.hpp"

#include <settings/ReadSettingsHelper.hpp>

/*!
 * Constructor. Gets the file name containing the control map description.
 */
MapController::MapController(FishBot* robot, QString controlMapFileName) :
    ExperimentController(robot, ExperimentControllerType::CONTROL_MAP, controlMapFileName)
{

}

/*!
 * Returns the control values for given position. If it's not in the map
 * then default (UNDEFINED) values are returned.
 */
ExperimentController::ControlData MapController::step(PositionMeters position)
{
    // values are initialized as undefined
    ControlData controlData;

    if (isValid() && position.isValid()) {
        QPointF point(position.x(), position.y());
        foreach (const ControlArea& controlArea, m_controlAreas) {
            if (controlArea.contains(point)) {
                controlData.controlMode = controlArea.controlMode();
                controlData.motionPattern = controlArea.motionPattern();
                return controlData;
            }
        }
    }
    return controlData;
}



