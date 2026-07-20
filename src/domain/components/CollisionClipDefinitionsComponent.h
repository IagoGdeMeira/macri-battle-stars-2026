#ifndef collision_clip_definitions_component_h
#define collision_clip_definitions_component_h

#include "CollisionFrame/CollisionFrame.h"
#include "StateId/StateId.h"

#include <memory>
#include <unordered_map>

struct CollisionClipDefinitionsComponent
{ std::unordered_map<StateId, std::shared_ptr<CollisionClip>, StateId::Hash> clips; };

#endif // collision_clip_definitions_component_h
