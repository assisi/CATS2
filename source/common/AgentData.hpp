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
class AgentData
{
public:
    //! Constructor.
    explicit AgentData(AgentType type, StateMeters stateMeters);

public:
    // TODO : add setters and getters

private:
    //! The type of image.
    AgentType _type;
    //! The position of the agent, in [m].
    StateMeters _stateMeters;
    //! The corresponding timestamp, in number of milliseconds since 1970-01-01T00:00:00
    //! Universal Coordinated Time. The timestamp is used to combine the tracking results
    //! from different concurrent trackers.
    std::chrono::milliseconds _timestamp;
};

#endif // CATS2_AGENT_DATA_HPP
