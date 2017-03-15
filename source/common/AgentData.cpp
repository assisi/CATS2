#include "AgentData.hpp"

/*!
 * Constructor.
 */
AgentDataWorld::AgentDataWorld(QString id, AgentType type, StateWorld stateWorld) :
    m_id(id),
    m_type(type),
    m_state(stateWorld)
{

}

/*!
 * Returns the label corresponding to this agent.
 */
QString AgentDataWorld::label() const
{
    QString typeString;
    switch (m_type) {
        case AgentType::CASU:
            typeString = "FishBot";
            break;
        case AgentType::FISH:
            typeString = "Fish";
            break;
        case AgentType::GENERIC:
            typeString = "Agent";
            break;
        case AgentType::UNDEFINED:
        default:
            typeString = "Unknown";
            break;
    }

    return QString("%1 %2").arg(typeString).arg(m_id);
}

/*!
 * Constructor.
 */
AgentDataImage::AgentDataImage(QString id, AgentType type, StateImage stateImage) :
    m_id(id),
    m_type(type),
    m_state(stateImage)
{

}
