#include "../include/StateMapper/StateMapper.h"

#include <stdexcept>

StateId StateMapper::fromString(const std::string& stateName)
{
    if (stateName == "Idle")            return StateId::Idle;
    if (stateName == "Running")         return StateId::Running;
    if (stateName == "Walking")         return StateId::Walking;
    if (stateName == "Jumping")         return StateId::Jumping;
    if (stateName == "Falling")         return StateId::Falling;
    if (stateName == "Punching")        return StateId::Punching;
    if (stateName == "Kicking")         return StateId::Kicking;
    if (stateName == "Blocking")        return StateId::Blocking;
    if (stateName == "Dying")           return StateId::Dying;
    if (stateName == "BeingHit")        return StateId::BeingHit;
    if (stateName == "ChargingSpecial") return StateId::ChargingSpecial;
    if (stateName == "SpecialAttack")   return StateId::SpecialAttack;
    
    throw std::invalid_argument("Unknown state name: " + stateName);
}
