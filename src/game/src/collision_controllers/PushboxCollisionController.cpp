#include "PushboxCollisionController.h"

#include "CollisionUtils/CollisionUtils.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/PushboxComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/components/PushboxControllerMapComponent.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"
#include "domain/utils/Logger/Logger.h"

bool PushboxCollisionController::hasMapComponent(const ControllerParams& params) const
{ return params.world.components().has<PushboxControllerMapComponent>(params.entity); }

bool PushboxCollisionController::hasController(const ControllerParams& params) const
{ return params.world.components().has<PushboxControllerComponent>(params.entity); }

void PushboxCollisionController::apply(const ControllerParams& params, StateId newState)
{
    auto& comp = params.world.components();
    if (!comp.has<PushboxControllerMapComponent>(params.entity)) return;

    auto& mapComp = comp.get<PushboxControllerMapComponent>(params.entity);
    auto it = mapComp.map.find(newState);
    if (it != mapComp.map.end()) this->applyController(params.entity, it->second, params.world);
    else this->remove(params);
}

void PushboxCollisionController::remove(const ControllerParams& params)
{
    auto& comp = params.world.components();
    if (!comp.has<PushboxControllerComponent>(params.entity)) return;
    
    auto& controller = comp.get<PushboxControllerComponent>(params.entity);
    this->deactivateCurrentFrame(controller, params.world);
    comp.remove<PushboxControllerComponent>(params.entity);
}

void PushboxCollisionController::onOrientationChanged(const ControllerParams& params)
{
    auto& comp = params.world.components();
    if (!comp.has<OrientationComponent>(params.entity)) return;

    Orientation orient = comp.get<OrientationComponent>(params.entity).direction;
    float sign = (orient == Orientation::Right) ? 1.0f : -1.0f;

    LOG_DEBUG("PushboxCollisionController::onOrientationChanged: entity {} orientation={} sign={}",
        params.entity.id, (orient == Orientation::Right ? "Right" : "Left"), sign);

    auto view = View<LocalTransform, ParentComponent>(comp);
    for (auto [childEntity, local, parent] : view)
    {
        if (parent.parent != params.entity) continue;
        if (!comp.has<PushboxComponent>(childEntity)) continue;

        local.position.x = std::abs(local.position.x) * sign;
        CollisionUtils::updateWorldTransform(params.world, childEntity, params.entity);
    }
}

void PushboxCollisionController::activateFirstFrame(PushboxControllerComponent& controller, World& world)
{
    if (controller.frames.empty() || controller.initialized) return;
    auto& comp = world.components();
    for (Entity e : controller.frames[0].pushboxes) if (comp.has<ActiveComponent>(e))
    { comp.get<ActiveComponent>(e).active = true; }
    controller.initialized = true;
}

void PushboxCollisionController::deactivateCurrentFrame(PushboxControllerComponent& controller, World& world)
{
    if (controller.frames.empty() || !controller.initialized) return;
    auto& comp = world.components();
    for (Entity e : controller.frames[controller.currentFrame].pushboxes)
    { if (comp.has<ActiveComponent>(e)) comp.get<ActiveComponent>(e).active = false; }
}

void PushboxCollisionController::applyController(Entity entity, const PushboxControllerComponent& newController, World& world)
{
    auto& comp = world.components();
    if (comp.has<PushboxControllerComponent>(entity))
    {
        auto& old = comp.get<PushboxControllerComponent>(entity);
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
        comp.add<PushboxControllerComponent>(entity, std::move(controller));
    }
}
