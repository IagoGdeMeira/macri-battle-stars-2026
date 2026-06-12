#ifndef state_id_mapper_h
#define state_id_mapper_h

#include "../../domain/include/StateId/StateId.h"

#include <cstdint>
#include <string>
#include <unordered_map>

class StateIdMapper
{
public:
    StateIdMapper() : nextCustomId(static_cast<std::int32_t>(StateId::BaseState::CustomStart)) {}

    StateId addCustomMapping(const std::string& name);
    void addCustomMapping(const std::string& name, StateId id);

    StateId fromString(const std::string& stateName) const;
    std::string toString(StateId id) const;

private:
    std::int32_t nextCustomId;

    std::unordered_map<std::string, StateId> customNameToId;
    std::unordered_map<std::int32_t, std::string> customIdToName;
};

#endif // state_id_mapper_h
