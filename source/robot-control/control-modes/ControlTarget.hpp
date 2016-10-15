#ifndef CATS2_CONTROL_TARGET_HPP
#define CATS2_CONTROL_TARGET_HPP

#include <AgentState.hpp>
#include <QtCore/QSharedPointer>

/*!
 * \brief The type of the target.
 */
enum class ControlTargetType
{
    SPEED,
    POSITION
};

/*!
 * \brief The generic control target class.
 */
class ControlTarget
{
public:
    //! Constructor.
    explicit ControlTarget(ControlTargetType type) : m_type(type) {}

public:
    //! Return the control target type.
    ControlTargetType type() const { return m_type; }

private:
    //! The type of control target.
    ControlTargetType m_type;
};

/*!
 * \brief Defines the target speed for the robot.
 */
class TargetSpeed : public ControlTarget
{
public:
    //! Constructor.
    explicit TargetSpeed(float leftSpeed = 0.0, float rightSpeed = 0.0) :
        ControlTarget(ControlTargetType::SPEED),
        m_leftSpeed(leftSpeed),
        m_rightSpeed(rightSpeed)
    {
    }

    //! Return the left motor speed.
    float leftSpeed() const { return m_leftSpeed; }
    //! Return the right motor speed.
    float rightSpeed() const { return m_rightSpeed; }

private:
    // TODO : define units
    //! Left motor target speed.
    float m_leftSpeed;
    //! Right motor target speed.
    float m_rightSpeed;
};


/*!
 * \brief Stores the target position for the robot.
 */
class TargetPosition : public ControlTarget
{
public:
    //! Constructor.
    explicit TargetPosition(PositionMeters position) :
        ControlTarget(ControlTargetType::POSITION),
        m_position(position)
    {
    }

    //! Return the target position.
    PositionMeters position() const { return m_position; }

private:
    //! The target position.
    PositionMeters m_position;
};

#endif // CATS2_CONTROL_TARGET_HPP

