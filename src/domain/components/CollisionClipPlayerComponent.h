#ifndef collision_clip_player_component_h
#define collision_clip_player_component_h

#include "../value_objects/CollisionFrame/CollisionFrame.h"

#include <memory>

struct CollisionClipPlayerComponent
{
    std::shared_ptr<CollisionClip> currentClip;
    int currentFrame = 0;
    float elapsedTime = 0.f;
    bool playing = false;
};

#endif // collision_clip_player_component_h
