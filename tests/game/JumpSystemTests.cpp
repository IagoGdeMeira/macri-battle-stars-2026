#include "../../src/game/include/JumpSystem/JumpSystem.h"

#include "../../src/domain/components/GroundedComponent.h"
#include "../../src/domain/components/HitstopComponent.h"
#include "../../src/domain/components/InputComponent.h"
#include "../../src/domain/components/PlayerComponent.h"
#include "../../src/domain/components/VelocityComponent.h"
#include "../../src/domain/value_objects/InputAction/InputAction.h"

#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/Scene/Scene.h"

#include "../../src/game/events/TriggerEvent.h"

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
        comp.registerComponent<PlayerComponent>();
        comp.registerComponent<VelocityComponent>();
    }
};

TEST_CASE_METHOD(JumpSystemFixture, "JumpSystem applies jump impulse and emits trigger when grounded",
    "[integration][jump_system]"
) {
    const auto& updateDelay = JumpSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Jump] = InputState{true, 0.f};

    GroundedComponent grounded;
    grounded.onGround = true;

    VelocityComponent velocity;

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<GroundedComponent>(entity, grounded);
    comp.add<VelocityComponent>(entity, velocity);
    comp.add<PlayerComponent>(entity, PlayerComponent{1});

    std::vector<TriggerEvent> events;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent& event) { events.push_back(event); });

    this->scene.systems().addSystem<JumpSystem>(this->bus, -420.f);
    this->scene.update(updateDelay);

    const auto& updatedVelocity = comp.get<VelocityComponent>(entity);
    REQUIRE(updatedVelocity.velocity.y == Catch::Approx(-420.f));
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
    input.actions[InputAction::Jump] = InputState{true, 0.f};

    GroundedComponent grounded;
    grounded.onGround = false;

    VelocityComponent velocity;
    velocity.velocity.y = 25.f;

    auto& components = this->scene.world().components();
    components.add<InputComponent>(entity, input);
    components.add<GroundedComponent>(entity, grounded);
    components.add<VelocityComponent>(entity, velocity);
    components.add<PlayerComponent>(entity, PlayerComponent{2});

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++eventCount; });

    this->scene.systems().addSystem<JumpSystem>(this->bus, -420.f);
    this->scene.update(updateDelay);

    const auto& updatedVelocity = components.get<VelocityComponent>(entity);
    REQUIRE(updatedVelocity.velocity.y == Catch::Approx(25.f));
    REQUIRE(eventCount == 0);
}

TEST_CASE_METHOD(JumpSystemFixture, "JumpSystem ignores jump input when entity is frozen by hitstop",
    "[integration][jump_system]"
) {
    const auto& updateDelay = JumpSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Jump] = InputState{true, 0.f};

    GroundedComponent grounded;
    grounded.onGround = true;

    VelocityComponent velocity;
    velocity.velocity.y = 18.f;

    HitstopComponent hitstop{ .remaining = 1.f, .frozen = true };

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<GroundedComponent>(entity, grounded);
    comp.add<VelocityComponent>(entity, velocity);
    comp.add<PlayerComponent>(entity, PlayerComponent{3});
    comp.add<HitstopComponent>(entity, hitstop);

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++eventCount; });

    this->scene.systems().addSystem<JumpSystem>(this->bus, -420.f);
    this->scene.update(updateDelay);

    const auto& updatedVelocity = comp.get<VelocityComponent>(entity);
    REQUIRE(updatedVelocity.velocity.y == Catch::Approx(18.f));
    REQUIRE(eventCount == 0);
}
