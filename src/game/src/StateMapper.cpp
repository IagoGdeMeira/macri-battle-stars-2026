#include "../include/StateMapper/StateMapper.h"

#include "../include/StateIdMapper/StateIdMapper.h"

#include <stdexcept>

StateId StateMapper::fromString(const std::string& stateName)
{
    static const StateIdMapper defaultMapper;

    const auto state = defaultMapper.fromString(stateName);
    if (state != StateId::Unknown) return state;

    throw std::invalid_argument("Unknown state name: " + stateName);
}
