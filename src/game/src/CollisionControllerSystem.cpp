#include "CollisionControllerSystem/CollisionControllerSystem.h"

#include "domain/components/StateComponent.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

CollisionControllerSystem::CollisionControllerSystem(EventBus& bus) : bus(bus)
{
    bus.subscribe<StateChangedEvent>([this](const StateChangedEvent& e)
    { this->stateChangedEvents.push_back(e); });

    bus.subscribe<OrientationChangedEvent>([this](const OrientationChangedEvent& e)
    { this->orientationChangedEvents.push_back(e); });
}

void CollisionControllerSystem::addController(std::unique_ptr<ICollisionController> controller)
{ this->controllers.push_back(std::move(controller)); }

void CollisionControllerSystem::update(UpdateContext& ctx)
{
    World& world = ctx.world;
    auto& comp = world.components();

    for (const auto& e : this->stateChangedEvents) for (auto& controller : this->controllers)
    {
        if (!controller->hasMapComponent({e.entity, world})) continue;
        controller->apply({e.entity, world}, e.current);
        controller->onOrientationChanged({e.entity, world});
    }
    this->stateChangedEvents.clear();

    for (const auto& e : this->orientationChangedEvents) for (auto& controller : this->controllers)
    { controller->onOrientationChanged({e.entity, world}); }

    this->orientationChangedEvents.clear();

    auto initView = View<StateComponent>(comp);
    for (auto [entity, state] : initView) for (auto& ctrl : this->controllers)
    {
        if (!ctrl->hasMapComponent({entity, world}) || ctrl->hasController({entity, world})) continue;

        ctrl->apply({entity, world}, state.current);
        ctrl->onOrientationChanged({entity, world});
    }
}
