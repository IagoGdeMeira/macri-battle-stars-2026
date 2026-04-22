#include "ColliderDef.h"

#include <memory>
#include <vector>

struct HitboxDef
{
    std::unique_ptr<ColliderDef> collider;
    int damage = 0;
};

struct HurtboxDef
{
    std::unique_ptr<ColliderDef> collider;
    float damageMultiplier = 1.0f;
};

struct CollisionFrame
{
    float duration = 0.0f;
    std::vector<HitboxDef> hitboxes;
    std::vector<HurtboxDef> hurtboxes;
};

struct CollisionClip
{
    std::vector<CollisionFrame> frames;
    bool loop = false;
};
