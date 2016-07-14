#ifndef CATS2_AGENT_DATA_HPP
#define CATS2_AGENT_DATA_HPP

#include "AgentState.hpp"

#include <chrono>

/*!
 * \brief The type of the agent.
 */
enum class AgentType
{
    FISH_CASU,
    FISH,
    UNDEFINED
};


/*!
 * \brief The class that contains states of agents on the arena.
 */
class AgentDataImage
{
public:
    //! Constructor.
    explicit AgentDataImage(unsigned char id,
                            AgentType type = AgentType::UNDEFINED,
                            StateImage stateImage = StateImage());

public:
    // TODO : add setters and getters

private:
    //! Agent's id.
    unsigned char _id;
    //! The type of image.
    AgentType _type;
    //! The position of the agent, in [px].
    StateImage _stateImage;
    //! The corresponding timestamp, in number of milliseconds since 1970-01-01T00:00:00
    //! Universal Coordinated Time. The timestamp is used to combine the tracking results
    //! from different concurrent trackers.
    std::chrono::milliseconds _timestamp;
};


/*!
 * \brief The class that contains states of agents on the arena.
 */
class AgentDataWorld
{
public:
    //! Constructor.
    explicit AgentDataWorld(unsigned char id,
                            AgentType type = AgentType::UNDEFINED,
                            StateWorld stateWorld = StateWorld());

public:
    // TODO : add setters and getters

private:
    //! Agent's id.
    unsigned char _id;
    //! The type of image.
    AgentType _type;
    //! The position of the agent, in [m].
    StateWorld _stateWorld;
    //! The corresponding timestamp, in number of milliseconds since 1970-01-01T00:00:00
    //! Universal Coordinated Time. The timestamp is used to combine the tracking results
    //! from different concurrent trackers.
    std::chrono::milliseconds _timestamp;
};

#endif // CATS2_AGENT_DATA_HPP
