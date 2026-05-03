#ifndef collision_clip_player_component_h
#define collision_clip_player_component_h

#include "../include/CollisionFrame/CollisionFrame.h"

#include <memory>

struct CollisionClipPlayerComponent
{
    std::shared_ptr<CollisionClip> currentClip;
    int currentFrame = 0;
    float elapsedTime = 0.0f;
    bool playing = false;
};

#endif // collision_clip_player_component_h
