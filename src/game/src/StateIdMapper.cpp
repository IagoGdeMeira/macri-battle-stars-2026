#include "../include/StateIdMapper/StateIdMapper.h"

#include <stdexcept>

StateId StateIdMapper::addCustomMapping(const std::string& name)
{
    const auto existing = this->customNameToId.find(name);
    if (existing != this->customNameToId.end()) return existing->second;
    
    const auto assignedId = this->nextCustomId++;
    const auto stateId = StateId::createCustom(assignedId);

    this->customNameToId[name] = stateId;
    this->customIdToName[assignedId] = name;

    return stateId;
}

void StateIdMapper::addCustomMapping(const std::string& name, StateId id)
{
    if (!id.isCustom()) throw std::invalid_argument("Custom mapping requires a custom StateId: " + name);

    this->customNameToId[name] = id;
    this->customIdToName[id.value()] = name;

    if (id.value() >= this->nextCustomId) this->nextCustomId = id.value() + 1;
}

StateId StateIdMapper::fromString(const std::string& stateName) const
{
    const auto base = StateId::fromBaseName(stateName);
    if (base != StateId::Unknown) return base;

    const auto custom = this->customNameToId.find(stateName);
    if (custom != this->customNameToId.end()) return custom->second;

    return StateId::Unknown;
}

std::string StateIdMapper::toString(StateId id) const
{
    if (!id.isCustom()) return StateId::toBaseName(id);

    const auto it = this->customIdToName.find(id.value());
    if (it != this->customIdToName.end()) return it->second;

    return "Unknown";
}
