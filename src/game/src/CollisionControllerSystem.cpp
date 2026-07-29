#include "CollisionControllerSystem/CollisionControllerSystem.h"

#include "domain/components/StateComponent.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"
#include "engine/include/EventBus/EventBus.h"
#include "domain/utils/Logger/Logger.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

CollisionControllerSystem::CollisionControllerSystem(EventBus& bus) : bus(bus)
{
    bus.subscribe<StateChangedEvent>([this](const StateChangedEvent& e)
    {
        LOG_DEBUG("CollisionControllerSystem: received StateChangedEvent entity {} prev {} curr {}",
            e.entity.id, StateId::toBaseName(e.previous), StateId::toBaseName(e.current));
        this->stateChangedEvents.push_back(e);
    });

    bus.subscribe<OrientationChangedEvent>([this](const OrientationChangedEvent& e)
    {
        LOG_DEBUG("CollisionControllerSystem: received OrientationChangedEvent entity {}",
            e.entity.id);
        this->orientationChangedEvents.push_back(e);
    });
}

void CollisionControllerSystem::addController(std::unique_ptr<ICollisionController> controller)
{ this->controllers.push_back(std::move(controller)); }

void CollisionControllerSystem::update(UpdateContext& ctx)
{
    World& world = ctx.world;
    auto& comp = world.components();

    for (const auto& e : this->stateChangedEvents) for (auto& controller : this->controllers)
    { if (controller->hasMapComponent({e.entity, world})) controller->apply({e.entity, world}, e.current); }
    this->stateChangedEvents.clear();

    for (const auto& e : this->orientationChangedEvents) for (auto& controller : this->controllers)
    { controller->onOrientationChanged({e.entity, world}); }
    this->orientationChangedEvents.clear();

    auto initView = View<StateComponent>(comp);
    for (auto [entity, state] : initView) for (auto& ctrl : this->controllers)
    {
        if (!ctrl->hasMapComponent({entity, world}) || ctrl->hasController({entity, world})) continue;
    
        LOG_DEBUG("CollisionControllerSystem: proactive init for entity {} state {}",
            entity.id, StateId::toBaseName(state.current));
        ctrl->apply({entity, world}, state.current);
    }
}
