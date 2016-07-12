#include "AgentData.hpp"

AgentData::AgentData(AgentType type, StateMeters stateMeters) :
    _type(type),
    _stateMeters(stateMeters),
    _timestamp(std::chrono::milliseconds())
{

}
