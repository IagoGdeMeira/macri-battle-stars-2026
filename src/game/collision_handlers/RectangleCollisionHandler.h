#ifndef rectangle_collision_handler_h
#define rectangle_collision_handler_h

#include "ICollisionHandler/ICollisionHandler.h"

#include "domain/components/TransformComponent.h"
#include "domain/components/RectangleColliderComponent.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <stdexcept>

class RectangleCollisionHandler : public ICollisionHandler
{
public:
    AABB getAABB(UpdateContext& ctx, EntityParams params) const override
    {
        auto& comp = ctx.world.components();
        
        const TransformComponent* transform = this->getTransformComponent(comp, { params.preferred, params.fallback });
        const RectangleColliderComponent* rect = this->getRectComponent(comp, { params.preferred, params.fallback });
        
        return this->buildAABB(*transform, *rect);
    }

    TransformComponent& getTransform(UpdateContext& ctx, EntityParams params) const override
    { return *this->getTransformComponent(ctx.world.components(), {params.preferred, params.fallback}); }

private:
    TransformComponent* getTransformComponent(ComponentManager& comp, EntityParams params) const
    {
        if (comp.has<TransformComponent>(params.preferred)) return &comp.get<TransformComponent>(params.preferred);

        if (params.fallback.has_value() && comp.has<TransformComponent>(*params.fallback))
        { return &comp.get<TransformComponent>(*params.fallback); }

        throw std::runtime_error("RectangleCollisionHandler: missing TransformComponent");
    }

    const RectangleColliderComponent* getRectComponent(ComponentManager& comp, EntityParams params) const
    {
        if (comp.has<RectangleColliderComponent>(params.preferred)) return &comp.get<RectangleColliderComponent>(params.preferred);
        
        if (params.fallback.has_value() && comp.has<RectangleColliderComponent>(*params.fallback))
        { return &comp.get<RectangleColliderComponent>(*params.fallback); }

        throw std::runtime_error("RectangleCollisionHandler: missing RectangleColliderComponent");
    }

    AABB buildAABB(const TransformComponent& t, const RectangleColliderComponent& r) const
    {
        float halfW = r.size.width * 0.5f, halfH = r.size.height * 0.5f;
        auto& pos = t.position;
        return AABB { pos.x - halfW, pos.x + halfW,  pos.y - halfH, pos.y + halfH };
    }
};

#endif // rectangle_collision_handler_h
