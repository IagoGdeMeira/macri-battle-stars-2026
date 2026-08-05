#include "HitboxCollisionController.h"

#include "CollisionUtils/CollisionUtils.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/HitboxComponent.h"
#include "domain/components/HitboxControllerComponent.h"
#include "domain/components/HitboxControllerMapComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"

bool HitboxCollisionController::hasMapComponent(const ControllerParams& params) const
{ return params.world.components().has<HitboxControllerMapComponent>(params.entity); }

bool HitboxCollisionController::hasController(const ControllerParams& params) const
{ return params.world.components().has<HitboxControllerComponent>(params.entity); }

void HitboxCollisionController::apply(const ControllerParams& params, StateId newState)
{
    auto& comp = params.world.components();
    if (!comp.has<HitboxControllerMapComponent>(params.entity)) return;

    auto& mapComp = comp.get<HitboxControllerMapComponent>(params.entity);
    auto it = mapComp.map.find(newState);
    if (it != mapComp.map.end()) this->applyController(params.entity, it->second, params.world);
    else this->remove(params);
}

void HitboxCollisionController::remove(const ControllerParams& params)
{
    auto& comp = params.world.components();
    if (!comp.has<HitboxControllerComponent>(params.entity)) return;
    
    auto& controller = comp.get<HitboxControllerComponent>(params.entity);
    this->deactivateCurrentFrame(controller, params.world);
    comp.remove<HitboxControllerComponent>(params.entity);
}

void HitboxCollisionController::onOrientationChanged(const ControllerParams& params)
{
    auto& comp = params.world.components();
    if (!comp.has<OrientationComponent>(params.entity)) return;
    if (!comp.has<HitboxControllerComponent>(params.entity)) return;

    auto& controller = comp.get<HitboxControllerComponent>(params.entity);
    if (controller.frames.empty() || !controller.initialized) return;

    for (Entity child : controller.frames[controller.currentFrame].hitboxes)
    {
        if (!comp.has<LocalTransform>(child) || !comp.has<HitboxComponent>(child)) continue;
        CollisionUtils::updateWorldTransform(params.world, child, params.entity);
    }
}

void HitboxCollisionController::activateFirstFrame(HitboxControllerComponent& controller, World& world)
{
    if (controller.frames.empty() || controller.initialized) return;
    auto& comp = world.components();
    for (Entity e : controller.frames[0].hitboxes) if (comp.has<ActiveComponent>(e))
    { comp.get<ActiveComponent>(e).active = true; }
    controller.initialized = true;
}

void HitboxCollisionController::deactivateCurrentFrame(HitboxControllerComponent& controller, World& world)
{
    if (controller.frames.empty() || !controller.initialized) return;
    auto& comp = world.components();
    for (Entity e : controller.frames[controller.currentFrame].hitboxes)
    { if (comp.has<ActiveComponent>(e)) comp.get<ActiveComponent>(e).active = false; }
}

void HitboxCollisionController::applyController(Entity entity, const HitboxControllerComponent& newController, World& world)
{
    auto& comp = world.components();
    if (comp.has<HitboxControllerComponent>(entity))
    {
        auto& old = comp.get<HitboxControllerComponent>(entity);
        this->deactivateCurrentFrame(old, world);
        old.frames = newController.frames;
        old.loop = newController.loop;
        old.currentFrame = 0;
        old.elapsedTime = 0.f;
        old.initialized = false;
        this->activateFirstFrame(old, world);
    }
    else
    {
        auto controller = newController;
        this->activateFirstFrame(controller, world);
        comp.add<HitboxControllerComponent>(entity, std::move(controller));
    }
}
