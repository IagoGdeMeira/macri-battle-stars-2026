#include "game/include/JumpSystem/JumpSystem.h"

#include "domain/components/GroundedComponent.h"
#include "domain/components/HitstopComponent.h"
#include "domain/components/InputComponent.h"
#include "domain/components/JumpComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/value_objects/InputAction/InputAction.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/Scene/Scene.h"

#include "game/events/TriggerEvent.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

class JumpSystemFixture
{
public:
    EventBus bus;
    Scene scene{this->bus};

    static constexpr float UPDATE_DELAY = 0.016f;

    JumpSystemFixture()
    {
        auto& comp = this->scene.world().components();
        comp.registerComponent<GroundedComponent>();
        comp.registerComponent<HitstopComponent>();
        comp.registerComponent<InputComponent>();
        comp.registerComponent<JumpComponent>();
        comp.registerComponent<VelocityComponent>();
    }
};

TEST_CASE_METHOD(JumpSystemFixture, "JumpSystem applies jump force over time and emits trigger when grounded",
    "[integration][jump_system]"
) {
    const auto& updateDelay = JumpSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Jump] = InputComponent::State{true, 0.f};

    GroundedComponent grounded;
    grounded.onGround = true;

    VelocityComponent velocity;
    JumpComponent jump;
    jump.force = 420.f;
    jump.maxTime = 0.5f;

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<GroundedComponent>(entity, grounded);
    comp.add<VelocityComponent>(entity, velocity);
    comp.add<JumpComponent>(entity, jump);

    std::vector<TriggerEvent> events;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent& event) { events.push_back(event); });

    this->scene.systems().addSystem<JumpSystem>(this->bus, -420.f);
    this->scene.update(updateDelay);

    const auto& updatedVelocity = comp.get<VelocityComponent>(entity);
    REQUIRE(updatedVelocity.velocity.y == Catch::Approx(-6.72f).margin(0.01f));
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].entity == entity);
    REQUIRE(events[0].trigger == TriggerId::Jump);
}

TEST_CASE_METHOD(JumpSystemFixture, "JumpSystem ignores jump input when entity is not grounded",
    "[integration][jump_system]"
) {
    const auto& updateDelay = JumpSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Jump] = InputComponent::State{true, 0.f};

    GroundedComponent grounded;
    grounded.onGround = false;

    VelocityComponent velocity;
    velocity.velocity.y = 25.f;

    JumpComponent jump;
    jump.force = 420.f;

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<GroundedComponent>(entity, grounded);
    comp.add<VelocityComponent>(entity, velocity);
    comp.add<JumpComponent>(entity, jump);

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++eventCount; });

    this->scene.systems().addSystem<JumpSystem>(this->bus, -420.f);
    this->scene.update(updateDelay);

    const auto& updatedVelocity = comp.get<VelocityComponent>(entity);
    REQUIRE(updatedVelocity.velocity.y == Catch::Approx(25.f));
    REQUIRE(eventCount == 0);
}

TEST_CASE_METHOD(JumpSystemFixture, "JumpSystem ignores jump input when entity is frozen by hitstop",
    "[integration][jump_system]"
) {
    const auto& updateDelay = JumpSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Jump] = InputComponent::State{true, 0.f};

    GroundedComponent grounded;
    grounded.onGround = true;

    VelocityComponent velocity;
    velocity.velocity.y = 18.f;

    HitstopComponent hitstop{ .remaining = 1.f, .frozen = true };

    JumpComponent jump;
    jump.force = 420.f;

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<GroundedComponent>(entity, grounded);
    comp.add<VelocityComponent>(entity, velocity);
    comp.add<JumpComponent>(entity, jump);
    comp.add<HitstopComponent>(entity, hitstop);

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++eventCount; });

    this->scene.systems().addSystem<JumpSystem>(this->bus, -420.f);
    this->scene.update(updateDelay);

    const auto& updatedVelocity = comp.get<VelocityComponent>(entity);
    REQUIRE(updatedVelocity.velocity.y == Catch::Approx(18.f));
    REQUIRE(eventCount == 0);
}
