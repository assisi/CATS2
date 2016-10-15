#include "ControlMode.hpp"
#include "FishBot.hpp"

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
