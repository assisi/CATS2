#ifndef CATS2_AGENT_DATA_ITEM_GROUP_HPP
#define CATS2_AGENT_DATA_ITEM_GROUP_HPP

#include <QtCore/QList>
#include <QtGui/QColor>

class TrajectoryItem;
class TargetItem;
class AnnotatedPolygonItem;

/*!
 * Unifies all the graphics items for an artificial agent. At
 */
struct AgentDataItemGroup
{
    //! Constructor.
    AgentDataItemGroup() :
        m_trajectory(nullptr),
        m_target(nullptr),
        m_areas()
    {

    }

    //! The agent's trajectory. Updated by the robot's navigation.
    TrajectoryItem* m_trajectory;
    //! The agent's current target. Updated by the robot's navigation.
    TargetItem* m_target;
    //! The agent's control areas.
    QList<AnnotatedPolygonItem*> m_areas;
};

#endif // CATS2_AGENT_DATA_ITEM_GROUP_HPP
