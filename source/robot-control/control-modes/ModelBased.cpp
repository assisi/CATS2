#include "ModelBased.hpp"
#include "FishBot.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
ModelBased::ModelBased(FishBot* robot) :
    ControlMode(robot, ControlModeType::MODEL_BASED),
    m_model()
{

}

/*!
 * The step of the control mode.
 */
ControlTargetPtr ModelBased::step()
{
    // if we track any fish
    if (m_robot->fishStates().size() > 0) {
        PositionMeters targetPosition = m_model.computeTarget(m_robot->fishStates());
        if (targetPosition.isValid()) {
            return ControlTargetPtr(new TargetPosition(targetPosition));
        }
    }
    // otherwise the robot doesn't move
    return ControlTargetPtr(new TargetSpeed(0, 0));
}

/*!
 * Informs on what kind of control targets this control mode generates.
 */
QList<ControlTargetType> ModelBased::supportedTargets()
{
    return QList<ControlTargetType>({ControlTargetType::POSITION});
}
