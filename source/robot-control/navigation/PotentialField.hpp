#ifndef CATS2_POTENTIAL_FIELD_HPP
#define CATS2_POTENTIAL_FIELD_HPP

#include "SetupMap.hpp"
#include "GridBasedMethod.hpp"
#include "settings/RobotControlSettings.hpp"

#include <opencv2/opencv.hpp>

class FishBot;

/*!
 * Runs the potential field method to detect and avoid obstacles. 
 * The method is run locally around the robot.
 */
class PotentialField : public GridBasedMethod
{
public:
    //! Constructor.
    PotentialField(FishBot* robot);
    //! Destructor.
    ~PotentialField();
    
    //! Updates the parameters of the potential field.
    void setSettings(PotentialFieldSettings settings);
    //! Returns the parameters of the potential field.
    const PotentialFieldSettings& settings() const { return m_settings; }

    //! Compute the total force on a robot, both attractive and repulsive.
    QVector2D computeTotalForceForRobot(PositionMeters targetPosition);

private:
    // FIXME FIXME: in all these method we never check that the robot's position
    // and the target's position are valid.
    //! Compute the repulsive force due to the arena.
    QVector2D computeLocalRepulsiveForceDueToArena();
    //! Compute the local repulsive force at a point due to an obstacle.
    QVector2D computeLocalRepulsiveForce(PositionMeters currentPos,
                                         PositionMeters obstaclePos);
    //! Compute the attractive force by the target.
    QVector2D computeAttractiveForce(PositionMeters targetPosition);
    //! Compute the repulsive force around a robot due to the other robots.
    QVector2D computeRepulsiveForceDueToRobots();

private:
    //! A pointer to the robot that is controlled by this method.
    FishBot* m_robot;

    //! Potential field settings.
    PotentialFieldSettings m_settings;
    // potential field parameters
    float m_nu,  m_rho0;
};

#endif // CATS2_POTENTIAL_FIELD_HPP
