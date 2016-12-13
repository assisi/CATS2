#include "PotentialField.hpp"

#include "FishBot.hpp"
#include "settings/RobotControlSettings.hpp"

#include <AgentData.hpp>

#include <QtGui/QVector2D>

/*!
 * Constructor.
 */
PotentialField::PotentialField(FishBot* robot) :
    m_robot(robot),
    GridBasedMethod(RobotControlSettings::get().pathPlanningSettings().gridSizeMeters()),
    m_zeta(2), m_dGoalStar(50),
    m_nuArena(300), m_rho0Arena(100),
    m_nuRobots(1000),m_rho0Robots(30),
    m_maxForce(1000),m_maxAngleDeg(60)
{
    m_configurationSpace = generateGrid();
}

/*! 
 * Updates the parameters of the potential field.
 */ 
void PotentialField::setParameters(int newNuRobots, int newRho0Robots,
                                   int newNuArena, int newRho0Arena,
                                   int newZeta, int newdGoalStar,
                                   int newMaxForce, int newMaxAngle)
{
    m_nuArena = newNuArena;
    m_rho0Arena = newRho0Arena;
    m_nuRobots = newNuRobots;
    m_rho0Robots = newRho0Robots;
    m_zeta = newZeta;
    m_dGoalStar = newdGoalStar;
    m_maxForce = newMaxForce;
    m_maxAngleDeg = newMaxAngle;
}

/*! 
 * Compute the repulsive force due to the arena.
 */ 
QVector2D PotentialField::computeLocalRepulsiveForceDueToArena()
{
    QVector2D arenaRepulsiveForce(0,0);
    QVector2D force(0,0);
    QPoint obstacleNode;

    QPoint currentPositionNode = positionToGridNode(m_robot->state().position());
    // iterate through the cells locally around current position
    for (int col = currentPositionNode.x() - DistInfluence / 2;
         col <= currentPositionNode.x() + DistInfluence / 2 ; ++col) {
        for (int row = currentPositionNode.y() - DistInfluence / 2 ;
             row <= currentPositionNode.y() + DistInfluence / 2 ; ++row) {

            // if the considered position is in bounds
            if ((col > 0) && (row > 0)
                    && (col < m_configurationSpace.cols)
                    && (row < m_configurationSpace.rows)) {
                // and if the current cell is occupied
                if(m_configurationSpace.at<uchar>(row, col) == GridStatus::OCCUPIED) {
                    // identify the cell as an obstacle
                    obstacleNode.setX(col);
                    obstacleNode.setY(row);

                    // intiailize repulsive force parameters
                    m_nu = m_nuArena;
                    m_rho0 = m_rho0Arena;
                    // compute the repulsive force
                    force = computeLocalRepulsiveForce(m_robot->state().position(),
                                                       gridNodeToPosition(obstacleNode));

                    // and sum it to the local repulsive force
                    arenaRepulsiveForce += force;
                }
            }
        }
    }
    return arenaRepulsiveForce;
}

/*!
 * Compute the local repulsive force at a point due to an obstacle.
 */
QVector2D PotentialField::computeLocalRepulsiveForce(PositionMeters robotPosition,
                                                     PositionMeters obstaclePosition)
{
    QVector2D localRepulsiveForce(0, 0);

    // compute the distance between the current position and the obstacle
    PositionMeters deltaPos = robotPosition - obstaclePosition;
    float distanceToObstable = robotPosition.distance2DTo(obstaclePosition);

    // if the distance is inferior to the limit
    if (distanceToObstable < m_rho0) {
        // compute the repulsive force
        localRepulsiveForce.setX(m_nu * (1 / distanceToObstable - 1 / m_rho0)
                                 * deltaPos.x() / pow(distanceToObstable, 3)) ;
        localRepulsiveForce.setY(m_nu * (1 / distanceToObstable - 1 / m_rho0)
                                 * deltaPos.y() / pow(distanceToObstable, 3)) ;
    }

    return localRepulsiveForce;
}

/*!
 * Compute the repulsive force around a robot due to the other robots.
 */
QVector2D PotentialField::computeRepulsiveForceDueToRobots()
{
    QVector2D robotsRepulsiveForce(0,0);
    PositionMeters currentPosition = m_robot->state().position();
    PositionMeters agentPosition;
    float orientationRad = m_robot->state().orientation().angleRad();

    // for all other robots
    for (const AgentDataWorld& agentData : m_robot->otherRobotsData()) {
            agentPosition = agentData.state().position();
            // compute angle between other agent and the robot
            float angleToAgentRad = qAtan2(agentPosition.y() - currentPosition.y(), 
                                        agentPosition.x() - currentPosition.x());
            float angleDifferenceRad = angleToAgentRad - orientationRad;

            // normalize angle
            if (angleDifferenceRad > M_PI)
                angleDifferenceRad -= 2 * M_PI;
            if (angleDifferenceRad < -M_PI)
                angleDifferenceRad += 2 * M_PI;

            // if the obstacle is in the robot's path
            if (qAbs(angleDifferenceRad * 180 / M_PI) < m_maxAngleDeg) {
                // compute the resulting force
                m_nu = m_nuRobots;
                m_rho0 = m_rho0Robots;
                robotsRepulsiveForce += computeLocalRepulsiveForce(currentPosition, agentPosition);
            }
    }
    return robotsRepulsiveForce;
}

/*!
 * Compute the attractive force by the target on it's robot.
 * FIXME : it's not computed on the grid, is it better?
 */
QVector2D PotentialField::computeAttractiveForce(PositionMeters targetPosition)
{
    QVector2D attractiveForce(0,0);
    float distanceToTarget;

    // compute the distance between current robot and it's target
    distanceToTarget = m_robot->state().position().distance2DTo(targetPosition);

    float deltaX = targetPosition.x() - m_robot->state().position().x();
    float deltaY = targetPosition.y() - m_robot->state().position().y();
    // if the distance is inferior to the limit
    if (distanceToTarget <= m_dGoalStar) {
        // compute the strong attractive force
        attractiveForce.setX(m_zeta * deltaX);
        attractiveForce.setY(m_zeta * deltaY);
    } else {
        // else compute the "weaker" attractive force
        attractiveForce.setX(m_dGoalStar * m_zeta * deltaX / distanceToTarget);
        attractiveForce.setY(m_dGoalStar * m_zeta * deltaY / distanceToTarget);
    }

    return attractiveForce;
}

/*!
 * Compute the total force on a robot, both attractive and repulsive.
 * Receive the current robot's target.
 */
QVector2D PotentialField::computeTotalForceForRobot(PositionMeters targetPosition)
{
    // compute the total repulsive force as the sum of the force due to the robots
    QVector2D robotRepulsiveForce = computeRepulsiveForceDueToRobots();

    // and the force due to the obstacles in the configuration space
    QVector2D arenaRepulsiveForce = computeLocalRepulsiveForceDueToArena();

    // the total attractive force at the current position
    QVector2D attractiveForce = computeAttractiveForce(targetPosition);

    // the total force is given by the total repulsive and attractive forces
    QVector2D totalForce =robotRepulsiveForce + arenaRepulsiveForce + attractiveForce;

    // compute the norm
    float forceNorm = totalForce.length();

    // if the norm is superior to the max force admissible, rescale.
    if (qAbs(forceNorm) > m_maxForce)
        totalForce *= m_maxForce / forceNorm;

    return totalForce;
}

