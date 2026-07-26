#ifndef hurtbox_controller_map_component_h
#define hurtbox_controller_map_component_h

#include "HurtboxControllerComponent.h"
#include "StateId/StateId.h"

#include <unordered_map>

struct HurtboxControllerMapComponent
{ std::unordered_map<StateId, HurtboxControllerComponent, StateId::Hash> map; };

#endif // hurtbox_controller_map_component_h
