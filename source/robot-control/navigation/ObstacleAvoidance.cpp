#include "ObstacleAvoidance.hpp"
#include "PotentialField.hpp"

#include <QtGui/QVector2D>

/*!  
 * Constructor.
 */
ObstacleAvoidance::ObstacleAvoidance(FishBot* robot) :
    m_potentialField(new PotentialField(robot))
{

}

/*!
 * Returns the target orientation.
 */
double ObstacleAvoidance::targetOrientationRad(PositionMeters targetPosition)
{
    QVector2D totalForce = m_potentialField->computeTotalForceForRobot(targetPosition);
    return qAtan2(totalForce.y(), totalForce.x());
}
