#include "AgentData.hpp"

/*!
 * Constructor.
 */
AgentDataWorld::AgentDataWorld(QString id, AgentType type, StateWorld stateWorld) :
    m_id(id),
    m_type(type),
    m_stateWorld(stateWorld),
    m_timestamp(std::chrono::milliseconds())
{

}

/*!
 * Constructor.
 */
AgentDataImage::AgentDataImage(QString id, AgentType type, StateImage stateImage) :
    m_id(id),
    m_type(type),
    m_stateImage(stateImage),
    m_timestamp(std::chrono::milliseconds())
{

}
