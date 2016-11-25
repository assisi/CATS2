#ifndef CATS2_NAVIGATION_HPP
#define CATS2_NAVIGATION_HPP

#include "RobotControlPointerTypes.hpp"
#include "MotionPatternType.hpp"
#include "PathPlanner.hpp"
#include "settings/RobotControlSettings.hpp"

#include <AgentState.hpp>

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
    //! Return the frequency divider for the motion pattern.
    int motionPatternFrequencyDivider(MotionPatternType::Enum type);

    //! The navigation step. Gets new control target and converts it to the
    //! motor commands.
    void step(ControlTargetPtr target);

signals:
    //! Informs that the robot's motion pattern was modified.
    void notifyMotionPatternChanged(MotionPatternType::Enum type);
    //! Informs that the value of the frequency divider of the motion
    //! pattern has been changed.
    void notifyMotionPatternFrequencyDividerChanged(MotionPatternType::Enum type,
                                                    int frequencyDivider);

public slots:
    //! Sets the requested motion pattern.
    void setMotionPattern(MotionPatternType::Enum type);
    //! Sets the frequency divider for the motion pattern. At the moment this
    //! is supported for the fish motion pattern only, but it can be used for
    //! other motion patterns as well.
    void setMotionPatternFrequencyDivider(MotionPatternType::Enum type,
                                          int frequencyDivider);

private:
    //! Manages the target speed control.
    void setTargetSpeed(TargetSpeed* targetSpeed);
    //! Manages the target position control.
    void setTargetPosition(TargetPosition* targetPostion);

    //! Computes the turn angle based on the robot's orientation,
    //! position and the target position.
    double computeAngleToTurn(PositionMeters position);
    //! Excecute fish motion pattern while going to target.
    void fishMotionToPosition(PositionMeters position);
    //! Excecutes PID while going to target.
    void pidControlToPosition(PositionMeters position);

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

    //! The path planner to the target position.
    PathPlanner m_pathPlanner;

    //! Local copy of fish motion pattern settings.
    FishMotionPatternSettings m_fishMotionPatternSettings;
    //! The frequency divider to send fish motion control commands to the robot
    //! less often.
    int m_fishMotionFrequencyDivider;
    //! The counter to apply the frequency divider.
    int m_fishMotionStepCounter;

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
