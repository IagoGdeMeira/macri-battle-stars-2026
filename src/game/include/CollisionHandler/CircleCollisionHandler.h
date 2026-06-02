#ifndef circle_collision_handler_h
#define circle_collision_handler_h

#include "ICollisionHandler.h"

#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/CircleColliderComponent.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <stdexcept>

class CircleCollisionHandler : public ICollisionHandler
{
public:
    AABB getAABB(UpdateContext& ctx, EntityParams params) const override
    {
        auto& comp = ctx.world.components();
        const TransformComponent* transform = this->getTransformComponent(
            comp, {params.preferred, params.fallback});
        const CircleColliderComponent* circle = this->getCircleComponent(
            comp, {params.preferred, params.fallback});
        return this->buildCircleAABB(*transform, *circle);
    }

    TransformComponent& getTransform(UpdateContext& ctx, EntityParams params) const override
    {
        auto& comp = ctx.world.components();
        return *this->getTransformComponent(comp, {params.preferred, params.fallback});
    }

private:
    TransformComponent* getTransformComponent(ComponentManager& comp, EntityParams params) const
    {
        if (comp.has<TransformComponent>(params.preferred))
        { return &comp.get<TransformComponent>(params.preferred); }
        
        if (params.fallback.has_value() && comp.has<TransformComponent>(*params.fallback))
        { return &comp.get<TransformComponent>(*params.fallback); }

        throw std::runtime_error("CircleCollisionHandler: missing TransformComponent");
    }

    const CircleColliderComponent* getCircleComponent(ComponentManager& comp, EntityParams params) const
    {
        if (comp.has<CircleColliderComponent>(params.preferred))
        { return &comp.get<CircleColliderComponent>(params.preferred); }

        if (params.fallback.has_value() && comp.has<CircleColliderComponent>(*params.fallback))
        { return &comp.get<CircleColliderComponent>(*params.fallback); }

        throw std::runtime_error("CircleCollisionHandler: missing CircleColliderComponent");
    }

    AABB buildCircleAABB(const TransformComponent& t, const CircleColliderComponent& c) const
    {
        float r = c.radius;
        auto& pos = t.position;
        return AABB { pos.x - r, pos.x + r, pos.y - r, pos.y + r };
    }
};

#endif // circle_collision_handler_h
