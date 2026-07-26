#ifndef hitbox_controller_map_component_h
#define hitbox_controller_map_component_h

#include "HitboxControllerComponent.h"
#include "StateId/StateId.h"

#include <unordered_map>

struct HitboxControllerMapComponent
{ std::unordered_map<StateId, HitboxControllerComponent, StateId::Hash> map; };

#endif // hitbox_controller_map_component_h
