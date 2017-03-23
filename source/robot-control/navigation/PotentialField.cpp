#include "PotentialField.hpp"

#include "FishBot.hpp"
#include "settings/RobotControlSettings.hpp"

#include <AgentData.hpp>

#include <QtGui/QVector2D>
#include <QtCore/QDebug>

/*!
 * Constructor.
 */
PotentialField::PotentialField(FishBot* robot) :
    m_robot(robot),
    GridBasedMethod(RobotControlSettings::get().pathPlanningSettings().gridSizeMeters()),
    m_settings(RobotControlSettings::get().potentialFieldSettings())
{
//    cv::namedWindow("PotentialFieldGrid", cv::WINDOW_NORMAL);
//    cv::imshow("PotentialFieldGrid", m_currentGrid);
}

/*!
 * Destructor.
 */
PotentialField::~PotentialField()
{
//    cv::destroyWindow("PotentialFieldGrid");
    qDebug() << "Destroying the object";
}

/*! 
 * Updates the parameters of the potential field.
 */ 
void PotentialField::setSettings(PotentialFieldSettings settings)
{
    m_settings = settings;
}

/*! 
 * Compute the repulsive force due to the arena.
 */ 
QVector2D PotentialField::computeLocalRepulsiveForceDueToArena()
{
    QVector2D arenaRepulsiveForce(0,0);
    QVector2D force(0,0);
    QPoint obstacleNode;

    if (m_robot->state().position().isValid()) {
        // the grid node closest to the robot's position
        QPoint robotNode = positionToGridNode(m_robot->state().position());
        // iterate through the cells locally around current position
        int areaGridDiameter = floor (m_settings.obstacleAvoidanceAreaDiameterMeters
                                      / m_gridSizeMeters + 0.5);
        for (int col = robotNode.x() - areaGridDiameter / 2;
             col <= robotNode.x() + areaGridDiameter / 2 ; ++col)
        {
            for (int row = robotNode.y() - areaGridDiameter / 2 ;
                 row <= robotNode.y() + areaGridDiameter / 2 ; ++row)
            {
                // if the considered position is in bounds
                if ((col > 0) && (row > 0)
                        && (col < m_currentGrid.cols)
                        && (row < m_currentGrid.rows)) {
                    // and if the current cell is occupied
                    if(m_currentGrid.at<uchar>(row, col) == GridStatus::OCCUPIED) {
                        // identify the cell as an obstacle
                        obstacleNode.setX(col);
                        obstacleNode.setY(row);
                        // intiailize repulsive force parameters
                        m_nu = m_settings.influenceStrengthArena;
                        m_rho0 = m_settings.influenceDistanceArenaMeters;
                        // compute the repulsive force
                        force = computeLocalRepulsiveForce(m_robot->state().position(),
                                                           gridNodeToPosition(obstacleNode));
                        // and sum it to the local repulsive force
                        arenaRepulsiveForce += force;
                    }
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
            if (qAbs(angleDifferenceRad * 180 / M_PI) < m_settings.maxAngleDeg) {
                // compute the resulting force
                m_nu = m_settings.influenceStrengthRobots;
                m_rho0 = m_settings.influenceDistanceRobotsMeters;
                robotsRepulsiveForce += computeLocalRepulsiveForce(currentPosition,
                                                                   agentPosition);
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
    if (distanceToTarget <= m_settings.influenceDistanceTargetMeters) {
        // compute the strong attractive force
        attractiveForce.setX(m_settings.influenceStrengthTarget * deltaX);
        attractiveForce.setY(m_settings.influenceStrengthTarget * deltaY);
    } else {
        // else compute the "weaker" attractive force
        attractiveForce.setX(m_settings.influenceDistanceTargetMeters *
                             m_settings.influenceStrengthTarget *
                             deltaX / distanceToTarget);
        attractiveForce.setY(m_settings.influenceDistanceTargetMeters *
                             m_settings.influenceStrengthTarget *
                             deltaY / distanceToTarget);
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
    if (qAbs(forceNorm) > m_settings.maxForce)
        totalForce *= m_settings.maxForce / forceNorm;

    return totalForce;
}

