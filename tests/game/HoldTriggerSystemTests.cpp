#include "game/include/HoldTriggerSystem/HoldTriggerSystem.h"

#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include "game/events/TriggerEvent.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

class HoldTriggerSystemFixture
{
public:
    HoldTriggerSystemFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<InputComponent>();
        comp.registerComponent<PlayerComponent>();
    }

    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};

    struct CollectedTrigger { Entity entity; TriggerId trigger; };
    std::vector<CollectedTrigger> receivedTriggers;

    void collectTriggers()
    {
        this->bus.subscribe<TriggerEvent>([this](const TriggerEvent& e)
        { this->receivedTriggers.push_back({e.entity, e.trigger}); });
    }

    Entity createPlayer(InputComponent input)
    {
        Entity entity = this->world.entities().create();
        auto& comp = this->world.components();
        comp.add<InputComponent>(entity, input);
        comp.add<PlayerComponent>(entity, PlayerComponent{1});
        return entity;
    }

    void setActionPressed(Entity entity, InputAction action, bool pressed)
    {
        auto& comp = this->world.components();
        auto& input = comp.get<InputComponent>(entity);
        input.actions[action].pressed = pressed;
    }
};

TEST_CASE_METHOD(HoldTriggerSystemFixture, "HoldTriggerSystem emits onPress when action is first pressed",
    "[unit][hold_trigger_system]")
{
    this->collectTriggers();
    HoldTriggerSystem system(this->bus, HoldTriggerSystem::Config{
        InputAction::Crouch, TriggerId::Crouched, TriggerId::CrouchReleased, nullptr });

    auto entity = this->createPlayer(InputComponent{});
    this->setActionPressed(entity, InputAction::Crouch, true);

    system.update(this->ctx);

    REQUIRE(this->receivedTriggers.size() == 1);
    REQUIRE(this->receivedTriggers[0].entity == entity);
    REQUIRE(this->receivedTriggers[0].trigger == TriggerId::Crouched);
}

TEST_CASE_METHOD(HoldTriggerSystemFixture, "HoldTriggerSystem does not emit onPress if already pressed",
    "[unit][hold_trigger_system]")
{
    this->collectTriggers();
    HoldTriggerSystem system(this->bus, HoldTriggerSystem::Config{
        InputAction::Crouch, TriggerId::Crouched, TriggerId::CrouchReleased });

    auto entity = this->createPlayer(InputComponent{});
    this->setActionPressed(entity, InputAction::Crouch, true);

    system.update(this->ctx);
    REQUIRE(this->receivedTriggers.size() == 1);
    this->receivedTriggers.clear();

    system.update(this->ctx);
    REQUIRE(this->receivedTriggers.empty());
}

TEST_CASE_METHOD(HoldTriggerSystemFixture, "HoldTriggerSystem emits onRelease when action is released",
    "[unit][hold_trigger_system]")
{
    this->collectTriggers();
    HoldTriggerSystem system(this->bus, HoldTriggerSystem::Config{
        InputAction::Crouch, TriggerId::Crouched, TriggerId::CrouchReleased });

    auto entity = this->createPlayer(InputComponent{});
    this->setActionPressed(entity, InputAction::Crouch, true);
    system.update(this->ctx);

    this->setActionPressed(entity, InputAction::Crouch, false);
    system.update(this->ctx);

    REQUIRE(this->receivedTriggers.size() == 2);
    REQUIRE(this->receivedTriggers[1].trigger == TriggerId::CrouchReleased);
}

TEST_CASE_METHOD(HoldTriggerSystemFixture, "HoldTriggerSystem does not emit onRelease if onRelease not configured",
    "[unit][hold_trigger_system]")
{
    this->collectTriggers();
    HoldTriggerSystem system(this->bus, HoldTriggerSystem::Config{
        InputAction::Punch, TriggerId::Punched, TriggerId::Unknown  });

    auto entity = this->createPlayer(InputComponent{});
    this->setActionPressed(entity, InputAction::Punch, true);
    system.update(this->ctx);

    this->setActionPressed(entity, InputAction::Punch, false);
    system.update(this->ctx);

    REQUIRE(this->receivedTriggers.size() == 1);
    REQUIRE(this->receivedTriggers[0].trigger == TriggerId::Punched);
}

TEST_CASE_METHOD(HoldTriggerSystemFixture, "HoldTriggerSystem does not emit onPress if condition returns false",
    "[unit][hold_trigger_system]")
{
    this->collectTriggers();
    HoldTriggerSystem system(this->bus, HoldTriggerSystem::Config{
        InputAction::Crouch, TriggerId::Crouched, TriggerId::CrouchReleased,
        [](UpdateContext&, Entity) -> bool { return false; } });

    auto entity = this->createPlayer(InputComponent{});
    this->setActionPressed(entity, InputAction::Crouch, true);

    system.update(this->ctx);
    REQUIRE(this->receivedTriggers.empty());
}

TEST_CASE_METHOD(HoldTriggerSystemFixture, "HoldTriggerSystem emits onPress again after release",
    "[unit][hold_trigger_system]")
{
    this->collectTriggers();
    HoldTriggerSystem system(this->bus, HoldTriggerSystem::Config{
        InputAction::Block, TriggerId::Blocked, TriggerId::BlockReleased });

    auto entity = this->createPlayer(InputComponent{});
    this->setActionPressed(entity, InputAction::Block, true);
    system.update(this->ctx);

    this->setActionPressed(entity, InputAction::Block, false);
    system.update(this->ctx);

    this->receivedTriggers.clear();

    this->setActionPressed(entity, InputAction::Block, true);
    system.update(this->ctx);

    REQUIRE(this->receivedTriggers.size() == 1);
    REQUIRE(this->receivedTriggers[0].trigger == TriggerId::Blocked);
}

TEST_CASE_METHOD(HoldTriggerSystemFixture, "HoldTriggerSystem does not emit onRelease if condition was false during press",
    "[unit][hold_trigger_system]")
{
    this->collectTriggers();
    bool allow = false;
    HoldTriggerSystem system(this->bus, HoldTriggerSystem::Config{
        InputAction::Crouch, TriggerId::Crouched, TriggerId::CrouchReleased,
        [&allow](UpdateContext&, Entity) { return allow; } });

    auto entity = this->createPlayer(InputComponent{});
    this->setActionPressed(entity, InputAction::Crouch, true);
    system.update(this->ctx);
    REQUIRE(this->receivedTriggers.empty());

    this->setActionPressed(entity, InputAction::Crouch, false);
    system.update(this->ctx);

    REQUIRE(this->receivedTriggers.empty());
}
