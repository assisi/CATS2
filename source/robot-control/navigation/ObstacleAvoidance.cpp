#include "ObstacleAvoidance.hpp"
#include "PotentialField.hpp"

/*!
 * Constructor.
 */
ObstacleAvoidance::ObstacleAvoidance(FishBot* robot) :
    m_potentialField(new PotentialField(robot))
{

}
