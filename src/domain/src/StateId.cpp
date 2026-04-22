#include "../include/StateId/StateId.h"

#include <stdexcept>

const StateId StateId::Unknown(StateId::BaseState::Unknown);
const StateId StateId::Idle(StateId::BaseState::Idle);
const StateId StateId::Running(StateId::BaseState::Running);
const StateId StateId::Walking(StateId::BaseState::Walking);
const StateId StateId::Jumping(StateId::BaseState::Jumping);
const StateId StateId::Falling(StateId::BaseState::Falling);
const StateId StateId::Punching(StateId::BaseState::Punching);
const StateId StateId::Kicking(StateId::BaseState::Kicking);
const StateId StateId::Blocking(StateId::BaseState::Blocking);
const StateId StateId::Dying(StateId::BaseState::Dying);
const StateId StateId::BeingHit(StateId::BaseState::BeingHit);
const StateId StateId::ChargingSpecial(StateId::BaseState::ChargingSpecial);
const StateId StateId::SpecialAttack(StateId::BaseState::SpecialAttack);

StateId StateId::createCustom(ValueType customId)
{
    if (customId < static_cast<ValueType>(BaseState::CustomStart))
    { throw std::invalid_argument("Custom StateId must be >= BaseState::CustomStart"); }

    return StateId(customId);
}

StateId StateId::fromBaseName(const std::string& name)
{
    const auto it = StateId::nameToId.find(name);
    if (it == StateId::nameToId.end()) return StateId::Unknown;
    
    return StateId(it->second);
}

std::string StateId::toBaseName(StateId id)
{
    const auto it = StateId::idToName.find(id.value());
    if (it == StateId::idToName.end()) return "Unknown";
    
    return it->second;
}

const std::unordered_map<std::string, StateId::BaseState> StateId::nameToId =
{
    { "Unknown", StateId::BaseState::Unknown },
    { "Idle", StateId::BaseState::Idle },
    { "Running", StateId::BaseState::Running },
    { "Walking", StateId::BaseState::Walking },
    { "Jumping", StateId::BaseState::Jumping },
    { "Falling", StateId::BaseState::Falling },
    { "Punching", StateId::BaseState::Punching },
    { "Kicking", StateId::BaseState::Kicking },
    { "Blocking", StateId::BaseState::Blocking },
    { "Dying", StateId::BaseState::Dying },
    { "BeingHit", StateId::BaseState::BeingHit },
    { "ChargingSpecial", StateId::BaseState::ChargingSpecial },
    { "SpecialAttack", StateId::BaseState::SpecialAttack }
};

const std::unordered_map<StateId::ValueType, std::string> StateId::idToName =
{
    { static_cast<StateId::ValueType>(StateId::BaseState::Unknown), "Unknown" },
    { static_cast<StateId::ValueType>(StateId::BaseState::Idle), "Idle" },
    { static_cast<StateId::ValueType>(StateId::BaseState::Running), "Running" },
    { static_cast<StateId::ValueType>(StateId::BaseState::Walking), "Walking" },
    { static_cast<StateId::ValueType>(StateId::BaseState::Jumping), "Jumping" },
    { static_cast<StateId::ValueType>(StateId::BaseState::Falling), "Falling" },
    { static_cast<StateId::ValueType>(StateId::BaseState::Punching), "Punching" },
    { static_cast<StateId::ValueType>(StateId::BaseState::Kicking), "Kicking" },
    { static_cast<StateId::ValueType>(StateId::BaseState::Blocking), "Blocking" },
    { static_cast<StateId::ValueType>(StateId::BaseState::Dying), "Dying" },
    { static_cast<StateId::ValueType>(StateId::BaseState::BeingHit), "BeingHit" },
    { static_cast<StateId::ValueType>(StateId::BaseState::ChargingSpecial), "ChargingSpecial" },
    { static_cast<StateId::ValueType>(StateId::BaseState::SpecialAttack), "SpecialAttack" }
};
