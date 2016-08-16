#ifndef CATS2_AGENT_DATA_HPP
#define CATS2_AGENT_DATA_HPP

#include "AgentState.hpp"

#include <opencv2/core/types.hpp>

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
    explicit AgentDataImage(QString id,
                            AgentType type = AgentType::UNDEFINED,
                            StateImage stateImage = StateImage());

public:
    //! Returns the agent's id.
    QString id() const { return m_id; }
    //! Returns the agent's type.
    AgentType type() const { return m_type; }

    //! Constant getter for the agent's state.
    const StateImage& state() const { return m_stateImage; }
    //! Mutable getter for the agent's state.
    StateImage* mutableState() { return &m_stateImage; }

    //! Sets the timestamp.
    void setTimestamp(std::chrono::milliseconds timestamp) { m_timestamp = timestamp; }

private:
    //! Agent's id.
    QString m_id;
    //! The type of image.
    AgentType m_type;
    //! The position of the agent (center) in [px], and it's orientation.
    StateImage m_stateImage;
    //! The corresponding timestamp, in number of milliseconds since 1970-01-01T00:00:00
    //! Universal Coordinated Time. The timestamp is used to combine the tracking results
    //! from different concurrent trackers.
    std::chrono::milliseconds m_timestamp;
};

/*!
 * \brief The class that contains states of agents on the arena.
 */
class AgentDataWorld
{
public:
    //! Constructor.
    explicit AgentDataWorld(QString id,
                            AgentType type = AgentType::UNDEFINED,
                            StateWorld stateWorld = StateWorld());

public:
    // TODO : add setters and getters

private:
    //! Agent's id.
    QString m_id;
    //! The type of image.
    AgentType m_type;
    //! The position of the agent, in [m].
    StateWorld m_stateWorld;
    //! The corresponding timestamp, in number of milliseconds since 1970-01-01T00:00:00
    //! Universal Coordinated Time. The timestamp is used to combine the tracking results
    //! from different concurrent trackers.
    std::chrono::milliseconds m_timestamp;
};

#endif // CATS2_AGENT_DATA_HPP
