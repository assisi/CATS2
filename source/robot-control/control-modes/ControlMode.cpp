#include "ControlMode.hpp"
#include "FishBot.hpp"
#include "ControlTarget.hpp"

/*!
 * Constructor.
 */
ControlMode::ControlMode(FishBot* robot, ControlModeType::Enum type):
    QObject(nullptr),
    m_robot(robot),
    m_type(type)
{

}

/*!
 * Destructor.
 */
ControlMode::~ControlMode()
{

}

/*!
 * Informs on what kind of control targets this control mode generates.
 */
QList<ControlTargetType> ControlMode::supportedTargets()
{
    return QList<ControlTargetType>();
}
