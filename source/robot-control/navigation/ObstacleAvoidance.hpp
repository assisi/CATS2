#ifndef CATS2_OBSTACLE_AVOIDANCE_HPP
#define CATS2_OBSTACLE_AVOIDANCE_HPP

#include "PotentialField.hpp"

class FishBot;

/*!
 * Runs the obstacle avoidance at every control step. At the moment only the
 * potential field method is used, but in future it is planned to be extended
 * with more of them.
 */
class ObstacleAvoidance
{
public:
    //! Constructor.
    ObstacleAvoidance(FishBot* robot);

private:
    //! Potential field obstacle avoidance method.
    PotentialField m_potentialField;
};
#endif // CATS2_OBSTACLE_AVOIDANCE_HPP
