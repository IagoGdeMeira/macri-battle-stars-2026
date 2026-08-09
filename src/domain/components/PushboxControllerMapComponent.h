#ifndef pushbox_controller_map_component_h
#define pushbox_controller_map_component_h

#include "PushboxControllerComponent.h"
#include "StateId/StateId.h"

#include <unordered_map>

struct PushboxControllerMapComponent
{ std::unordered_map<StateId, PushboxControllerComponent, StateId::Hash> map; };

#endif // pushbox_controller_map_component_h
