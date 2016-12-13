#ifndef CATS2_POTENTIAL_FIELD_HPP
#define CATS2_POTENTIAL_FIELD_HPP

#include "SetupMap.hpp"
#include "GridBasedMethod.hpp"

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
    
    //! Updates the parameters of the potential field.
    void setParameters(int newNuRobots, int newRho0Robots,
                                       int newNuArena, int newRho0Arena,
                                       int newZeta, int newdGoalStar,
                                       int newMaxForce, int newMaxAngle);    

    //! Compute the total force on a robot, both attractive and repulsive.
    QVector2D computeTotalForceForRobot(PositionMeters targetPosition);

private:
    // FIXME : in all these method we never check that the robot's position
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
    
    //! Contains the discretized configuration space
    cv::Mat m_configurationSpace;

    // potential field parameters
    //! Repulsive Parameters rho0 being the distance of influence and nu the "strength" of the repulsion
    float m_rho0Arena,  m_nuArena;
    float m_rho0Robots, m_nuRobots;
    float m_nu,  m_rho0;
    //! Attractive Parameters dgoalstar being the distance of influence and zeta the "strength" of the attraction
    float m_zeta;
    float m_dGoalStar;
    //! General Parameters
    float m_maxForce;
    float m_maxAngleDeg;
    // TODO : to make a parameter?
    const int DistInfluence = 150;
    const int RobotsInfluence = 40;
};

#endif // CATS2_POTENTIAL_FIELD_HPP
