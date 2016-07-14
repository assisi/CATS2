#include "AgentData.hpp"

/*!
 * Constructor.
 */
AgentDataWorld::AgentDataWorld(unsigned char id, AgentType type, StateWorld stateWorld) :
    _id(id),
    _type(type),
    _stateWorld(stateWorld),
    _timestamp(std::chrono::milliseconds())
{

}

/*!
 * Constructor.
 */
AgentDataImage::AgentDataImage(unsigned char id, AgentType type, StateImage stateImage) :
    _id(id),
    _type(type),
    _stateImage(stateImage),
    _timestamp(std::chrono::milliseconds())
{

}
