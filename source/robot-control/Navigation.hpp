#ifndef CATS2_NAVIGATION_HPP
#define CATS2_NAVIGATION_HPP

#include "RobotControlPointerTypes.hpp"
#include "MotionPatternType.hpp"
#include "settings/RobotControlSettings.hpp"

#include <QtCore/QObject>
#include <QtCore/QQueue>

class TargetSpeed;
class TargetPosition;

/*!
 * Conversts the current control target into the motor command and sends it to
 * the robot.
 */
class Navigation : public QObject
{
    Q_OBJECT
public:
    //! Constructor. Gets the robot interface to send commands.
    explicit Navigation(FishBot* robot);
    //! Destructor.
    virtual ~Navigation() final;

    //! Return the current motion pattern.
    MotionPatternType::Enum motionPattern() const { return m_motionPattern; }

    //! The navigation step. Gets new control target and converts it to the
    //! motor commands.
    void step(ControlTargetPtr target);

signals:
    //! Informs that the robot's motion pattern was modified.
    void notifyMotionPatternChanged(MotionPatternType::Enum type);

public slots:
    //! Sets the requested motion pattern.
    void setMotionPattern(MotionPatternType::Enum type);

private:
    //! Manages the target speed control.
    void setTargetSpeed(TargetSpeed* targetSpeed);
    //! Manages the target position control.
    void setTargetPosition(TargetPosition* targetPostion);

    //! Excecute fish motion pattern while going to target.
    void fishMotionToTargetPosition(TargetPosition* targetPostion);
    //! Excecutes PID while going to target.
    void pidControlToTargetPosition(TargetPosition* targetPostion);

    //! Sends the motor speed to the robot.
    void sendMotorSpeed(int leftSpeed, int rightSpeed);
    //! Sends the motor speed to the robot. It's converted from the angular speed.
    void sendMotorSpeed(double angularSpeedCmSec);
    //! Sends the fish behavior parameters to the robot.
    void sendFishMotionParameters(int angle, int distance, int speed);

private:
    //! A pointer to the robot that is controlled by this navigation.
    FishBot* m_robot;

    //! The motion pattern used to control the robot.
    MotionPatternType::Enum m_motionPattern;

    //! Local copy of fish motion pattern settings.
    FishMotionPatternSettings m_fishMotionPatternSettings;
    //! Local copy of PID controller settings.
    PidControllerSettings m_pidControllerSettings;

    //! The last several errors, to be used by the intergral term of the PID
    //! controller
    QQueue<double> m_errorBuffer;
    //! The number of errors to keep.
    const int ErrorBufferDepth = 5;
    //! The control loop duration.
    double m_dt;
};

#endif // CATS2_NAVIGATION_HPP
