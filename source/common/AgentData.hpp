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
    GENERIC,    // any type of the agent, different from UNDEFINED
    UNDEFINED   // unknown, no data
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
    const StateImage& state() const { return m_state; }
    //! Mutable getter for the agent's state.
    StateImage* mutableState() { return &m_state; }

    //! Sets the timestamp.
    void setTimestamp(std::chrono::milliseconds timestamp) { m_timestamp = timestamp; }
    //! Returns the timastamp.
    std::chrono::milliseconds timestamp() const { return m_timestamp; }

private:
    //! Agent's id.
    QString m_id;
    //! The type of image.
    AgentType m_type;
    //! The position of the agent (center) in [px], and it's orientation.
    StateImage m_state;
    //! The corresponding timestamp, in number of milliseconds since 1970-01-01T00:00:00
    //! Universal Coordinated Time. The timestamp is to be used to detect the
    //! outdated data when tracking (assigning ids) to the agents.
    std::chrono::milliseconds m_timestamp;
};

/*!
 * \brief The class that contains states of agents on the arena.
 * NOTE : this class doesn't contain the timastamp because world positions
 * are always secondary to the image positions, and thus are not directly used
 * in the tracking routines.
 */
class AgentDataWorld
{
public:
    //! Constructor.
    explicit AgentDataWorld(QString id,
                            AgentType type = AgentType::UNDEFINED,
                            StateWorld stateWorld = StateWorld());

public:
    //! Returns the agent's id.
    QString id() const { return m_id; }
    //! Returns the label corresponding to this agent.
    QString label() const;

    //! Returns the agent's type.
    AgentType type() const { return m_type; }
    //! Set the agent's type.
    void setType(AgentType type) { m_type = type; }

    //! Constant getter for the agent's state.
    const StateWorld& state() const { return m_state; }

private:
    //! Agent's id.
    QString m_id;
    //! The type of image.
    AgentType m_type;
    //! The position of the agent, in [m].
    StateWorld m_state;
};

struct TimestampedWorldAgentData
{
    //! The agent's data.
    QList<AgentDataWorld> agentsData;
    //! The corresponding timestamp, in number of milliseconds since 1970-01-01T00:00:00
    //! Universal Coordinated Time. The timestamp is used to combine the tracking results
    //! from different concurrent trackers.
    std::chrono::milliseconds timestamp;
};
#endif // CATS2_AGENT_DATA_HPP
