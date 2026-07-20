#ifndef collision_frame_h
#define collision_frame_h

#include "ColliderDef.h"
#include "Color/Color.h"
#include "PushboxComponent.h"

#include <memory>
#include <vector>

struct ColliderDebugDef { Color color = {255, 255, 255, 0}; bool enabled = false; };

struct HitboxDef
{
    std::unique_ptr<ColliderDef> collider;
    int damage = 0;
    ColliderDebugDef debug;
};

struct HurtboxDef
{
    std::unique_ptr<ColliderDef> collider;
    float damageMultiplier = 1.f;
    ColliderDebugDef debug;
};

struct PushboxDef
{
    using Type = PushboxComponent::Type;

    std::unique_ptr<ColliderDef> collider;
    Type type = Type::Dynamic;
    float mass = 1.f, pushResistance = 1.f;
    ColliderDebugDef debug;
};

struct CollisionFrame
{
    float duration = 0.f;
    std::vector<HitboxDef> hitboxes;
    std::vector<HurtboxDef> hurtboxes;
    std::vector<PushboxDef> pushboxes;
};

struct CollisionClip { std::vector<CollisionFrame> frames; bool loop = false; };

#endif // collision_frame_h
