#include "HurtboxCollisionController.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/HurtboxControllerComponent.h"
#include "domain/components/HurtboxControllerMapComponent.h"
#include "domain/include/World/World.h"

bool HurtboxCollisionController::hasMapComponent(const ControllerParams& params) const
{ return params.world.components().has<HurtboxControllerMapComponent>(params.entity); }

bool HurtboxCollisionController::hasController(const ControllerParams& params) const
{ return params.world.components().has<HurtboxControllerComponent>(params.entity); }

void HurtboxCollisionController::apply(const ControllerParams& params, StateId newState)
{
    auto& comp = params.world.components();
    if (!comp.has<HurtboxControllerMapComponent>(params.entity)) return;

    auto& mapComp = comp.get<HurtboxControllerMapComponent>(params.entity);
    auto it = mapComp.map.find(newState);
    if (it != mapComp.map.end()) this->applyController(params.entity, it->second, params.world);
    else this->remove(params);
}

void HurtboxCollisionController::remove(const ControllerParams& params)
{
    auto& comp = params.world.components();
    if (comp.has<HurtboxControllerComponent>(params.entity))
    {
        auto& controller = comp.get<HurtboxControllerComponent>(params.entity);
        this->deactivateCurrentFrame(controller, params.world);
        comp.remove<HurtboxControllerComponent>(params.entity);
    }
}

void HurtboxCollisionController::activateFirstFrame(HurtboxControllerComponent& controller, World& world)
{
    if (controller.frames.empty() || controller.initialized) return;
    auto& comp = world.components();
    for (Entity e : controller.frames[0].hurtboxes) if (comp.has<ActiveComponent>(e))
    { comp.get<ActiveComponent>(e).active = true; }
    controller.initialized = true;
}

void HurtboxCollisionController::deactivateCurrentFrame(HurtboxControllerComponent& controller, World& world)
{
    if (controller.frames.empty() || !controller.initialized) return;
    auto& comp = world.components();
    for (Entity e : controller.frames[controller.currentFrame].hurtboxes)
    { if (comp.has<ActiveComponent>(e)) comp.get<ActiveComponent>(e).active = false; }
}

void HurtboxCollisionController::applyController(Entity entity, const HurtboxControllerComponent& newController, World& world)
{
    auto& comp = world.components();
    if (comp.has<HurtboxControllerComponent>(entity))
    {
        auto& old = comp.get<HurtboxControllerComponent>(entity);
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
        comp.add<HurtboxControllerComponent>(entity, std::move(controller));
    }
}
