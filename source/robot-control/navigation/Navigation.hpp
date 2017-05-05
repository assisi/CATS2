#ifndef CATS2_NAVIGATION_HPP
#define CATS2_NAVIGATION_HPP

#include "RobotControlPointerTypes.hpp"
#include "MotionPatternType.hpp"
#include "PathPlanner.hpp"
#include "ObstacleAvoidance.hpp"
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
    //! Requests the robot to stop.
    void stop();

    //! Returns the path planning usage flag.
    bool usePathPlanning() const { return m_usePathPlanning; }

    //! Returns the obstacle avoidance usage flag.
    bool useObstacleAvoidance() const { return m_useObstacleAvoidance; }

public:
    // FIXME : this is a temporary code, to be removed once the parameters of the
    // obstacle avoidance are tuned
    //! Returns a pointer to the potential field obstacle avoidance module.
    PotentialFieldPtr potentialField() { return m_obstacleAvoidance.potentialField(); }

public:
    //! Sets the requested motion pattern.
    void setMotionPattern(MotionPatternType::Enum type);
    //! Sets the frequency divider for the motion pattern. At the moment this
    //! is supported for the fish motion pattern only, but it can be used for
    //! other motion patterns as well.
    void setMotionPatternFrequencyDivider(MotionPatternType::Enum type,
                                          int frequencyDivider);
    //! Sets the path planning usage flag.
    void setUsePathPlanning(bool usePathPlanning);
    //! Sets the obstacle avoidance usage flag.
    void setUseObstacleAvoidance(bool useObstacleAvoidance);

    //! Sets the local obstacle avoidance on the robot.
    void updateLocalObstacleAvoidance();

public:
    //! The type of the on-board obstacle avoidance
    enum LocalObstacleAvoidanceType
    {
        DISABLED,
        BRAITENBERG,
        TURN_AND_GO
    };

public slots:
    //! Requests to sends the current target.
    void requestTargetPosition() { emit notifyTargetPositionChanged(m_currentWaypoint); }
    //! Requests to sends the current trajectory.
    void requestTrajectory() { m_pathPlanner.requestTrajectory(); }

signals:
    //! Informs that the robot's motion pattern was modified.
    void notifyMotionPatternChanged(MotionPatternType::Enum type);
    //! Informs that the value of the frequency divider of the motion
    //! pattern has been changed.
    void notifyMotionPatternFrequencyDividerChanged(MotionPatternType::Enum type,
                                                    int frequencyDivider);
    //! Informs that the robot's target position was modified.
    void notifyTargetPositionChanged(PositionMeters PositionMeters);
    //! Informs that the path planning is on/off in the navigation.
    void notifyUsePathPlanningChanged(bool value);
    //! Informs that the obstacle avoidance is on/off in the navigation.
    void notifyUseObstacleAvoidanceChanged(bool value);
    //! Informs that the robot's trajectory has changed.
    void notifyTrajectoryChanged(QQueue<PositionMeters>);
    
private:
    //! Manages the target speed control.
    void setTargetSpeed(TargetSpeed* targetSpeed);
    //! Manages the target position control.
    void setTargetPosition(TargetPosition* targetPostion);
    //! Manages the local obstacle avoidance on the robot.
    void setLocalObstacleAvoidanceForMotionPattern(MotionPatternType::Enum motionPattern);

    //! Computes the turn angle based on the robot's orientation,
    //! position and the target position.
    double computeAngleToTurn(PositionMeters position);
    //! Computes the distance between the target and the current robot's
    //! position.
    double computeDistanceToTarget(PositionMeters targetPosition);
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
    //! Sends the local obstacle type to the robot.
    void sendLocalObstacleAvoidance(LocalObstacleAvoidanceType type);

    //! Updates the current waypoint. Notifies on the change.
    void updateCurrentWaypoint(PositionMeters currentWaypoint);

private:
    //! A pointer to the robot that is controlled by this navigation.
    FishBot* m_robot;

    //! The motion pattern used to control the robot.
    MotionPatternType::Enum m_motionPattern;

    //! The path planner to the target position.
    PathPlanner m_pathPlanner;
    //! The flag that says if the path planning is to be used.
    bool m_usePathPlanning;
    //! The current waypoint to follow. It's stored to be given upon request.
    PositionMeters m_currentWaypoint;

    //! The obstacle avoidance module.
    ObstacleAvoidance m_obstacleAvoidance;
    //! The flag that says if the obstacle avoidance is to be used.
    bool m_useObstacleAvoidance;

    //! If the robot needs to have a valid orientation to navigate. It's
    //! important when the precise navigation is required, but might be very
    //! restrictive as the robot won't move untill it's orientation is known.
    bool m_needOrientationToNavigate;
    //! The robot stops when it arrives to the target. The fish never stop and
    //! thus this flag was introduced.
    bool m_stopOnceOnTarget;

    //! Local copy of fish motion pattern settings.
    FishMotionPatternSettings m_fishMotionPatternSettings;
    //! The frequency divider to send fish motion control commands to the robot
    //! less often.
    int m_fishMotionFrequencyDivider;
    //! The counter to apply the frequency divider.
    int m_fishMotionStepCounter;

    //! Local copy of PID controller settings.
    PidControllerSettings m_pidControllerSettings;

    //! The last several errors on the angle, to be used by the intergral term
    //! of the PID controller
    QQueue<double> m_errorBufferAngle;

    //! The last several errors on distance, to be used by the intergral term
    //! of the PID controller
    QQueue<double> m_errorBufferDistance;

    //! The number of errors to keep.
    const int ErrorBufferDepth = 20;

    //! The control loop duration.
    double m_dt;
};

#endif // CATS2_NAVIGATION_HPP
