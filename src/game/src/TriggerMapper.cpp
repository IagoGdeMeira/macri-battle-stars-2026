#include "../include/TriggerMapper/TriggerMapper.h"

#include <stdexcept>

TriggerId TriggerMapper::fromString(const std::string& triggerName)
{
    if (triggerName == "Jumped")                    return TriggerId::Jumped;
    if (triggerName == "Landed")                    return TriggerId::Landed;
    if (triggerName == "Kicked")                    return TriggerId::Kicked;
    if (triggerName == "Punched")                   return TriggerId::Punched;
    if (triggerName == "Blocked")                   return TriggerId::Blocked;
    if (triggerName == "StartedChargingSpecial")    return TriggerId::StartedChargingSpecial;
    if (triggerName == "ReleasedSpecial")           return TriggerId::ReleasedSpecial;
    if (triggerName == "TookDamage")                return TriggerId::TookDamage;
    if (triggerName == "Died")                      return TriggerId::Died;
    if (triggerName == "TouchedWall")               return TriggerId::TouchedWall;
    if (triggerName == "TouchedCeiling")            return TriggerId::TouchedCeiling;
    if (triggerName == "LookedUp")                  return TriggerId::LookedUp;
    if (triggerName == "LookedDown")                return TriggerId::LookedDown;
    if (triggerName == "LookedLeft")                return TriggerId::LookedLeft;
    if (triggerName == "LookedRight")               return TriggerId::LookedRight;

    throw std::invalid_argument("Unknown trigger name: " + triggerName);
}
