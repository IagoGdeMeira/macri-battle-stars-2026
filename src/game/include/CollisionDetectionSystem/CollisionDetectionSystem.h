#ifndef collision_detection_system_h
#define collision_detection_system_h

#include "../../engine/include/System/System.h"

#include "../../domain/include/Entity/Entity.h"

#include <vector>

struct CircleColliderComponent;
struct RectangleColliderComponent;
struct TransformComponent;

class CollisionDetectionSystem : public System
{
public:
    void update(UpdateContext& ctx) override;

    struct RectParams
    {
        const TransformComponent& transform;
        const RectangleColliderComponent& collider;
    };

    struct CircleParams
    {
        const TransformComponent& transform;
        const CircleColliderComponent& collider;
    };

private:
    struct CollisionPair { Entity a, b; };

    using RectP = CollisionDetectionSystem::RectParams;
    using CircleP = CollisionDetectionSystem::CircleParams;

    static bool rectToRect(RectP a, RectP b);
    static bool circleToCircle(CircleP a, CircleP b);
    static bool rectToCircle(RectP rect, CircleP circle);

    void detect(UpdateContext& ctx, std::vector<CollisionPair>& pairs);
};

#endif // collision_detection_system_h
