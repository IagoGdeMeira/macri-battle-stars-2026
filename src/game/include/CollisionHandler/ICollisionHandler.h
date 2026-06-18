#ifndef i_collision_handler_h
#define i_collision_handler_h

#include "../../domain/include/Entity/Entity.h"
#include "../../domain/value_objects/Geometry/Geometry.h"

#include <optional>

struct TransformComponent;
struct UpdateContext;

class ICollisionHandler
{
public:
    struct EntityParams { Entity preferred; std::optional<Entity> fallback; };

    virtual ~ICollisionHandler() = default;

    virtual AABB getAABB(UpdateContext& ctx, EntityParams params) const = 0;
    virtual TransformComponent& getTransform(UpdateContext& ctx, EntityParams params) const = 0;
};

#endif // i_collision_handler_h
