#ifndef rectangle_collision_handler_h
#define rectangle_collision_handler_h

#include "ICollisionHandler.h"

#include "../../../domain/components/TransformComponent.h"
#include "../../../domain/components/RectangleColliderComponent.h"

#include "../../../engine/include/UpdateContext/UpdateContext.h"

#include <stdexcept>

class RectangleCollisionHandler : public ICollisionHandler
{
public:
    AABB getAABB(UpdateContext& ctx, EntityParams params) const override
    {
        auto& comp = ctx.world.components();
        const TransformComponent* transform = this->getTransformComponent(
            comp, { params.preferred, params.fallback });
        const RectangleColliderComponent* rect = this->getRectComponent(
            comp, { params.preferred, params.fallback });
        return this->buildAABB(*transform, *rect);
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

        throw std::runtime_error("RectangleCollisionHandler: missing TransformComponent");
    }

    const RectangleColliderComponent* getRectComponent(ComponentManager& comp, EntityParams params) const
    {
        if (comp.has<RectangleColliderComponent>(params.preferred))
        { return &comp.get<RectangleColliderComponent>(params.preferred); }
        
        if (params.fallback.has_value() && comp.has<RectangleColliderComponent>(*params.fallback))
        { return &comp.get<RectangleColliderComponent>(*params.fallback); }

        throw std::runtime_error("RectangleCollisionHandler: missing RectangleColliderComponent");
    }

    AABB buildAABB(const TransformComponent& t, const RectangleColliderComponent& r) const
    {
        float halfW = r.width * 0.5f, halfH = r.height * 0.5f;
        return AABB { t.x - halfW, t.x + halfW,  t.y - halfH, t.y + halfH };
    }
};

#endif // rectangle_collision_handler_h
