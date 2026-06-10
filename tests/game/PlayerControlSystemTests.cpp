#include "../../src/game/include/PlayerControlSystem/PlayerControlSystem.h"

#include "../../src/domain/components/AnalogInputComponent.h"
#include "../../src/domain/components/GroundedComponent.h"
#include "../../src/domain/components/HitstopComponent.h"
#include "../../src/domain/components/InputComponent.h"
#include "../../src/domain/components/PlayerComponent.h"
#include "../../src/domain/components/StateComponent.h"
#include "../../src/domain/components/VelocityComponent.h"

#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/Scene/Scene.h"

#include "../../src/game/events/TriggerEvent.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

class PlayerControlSystemFixture
{
public:
    EventBus bus;
    Scene scene{this->bus};

    PlayerControlSystemFixture()
    {
        auto& components = this->scene.world().components();
        components.registerComponent<AnalogInputComponent>();
        components.registerComponent<GroundedComponent>();
        components.registerComponent<HitstopComponent>();
        components.registerComponent<InputComponent>();
        components.registerComponent<PlayerComponent>();
        components.registerComponent<StateComponent>();
        components.registerComponent<VelocityComponent>();
    }
};

TEST_CASE_METHOD(PlayerControlSystemFixture, "PlayerControlSystem moves, jumps and emits combat triggers",
    "[integration][player_control_system]"
) {
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Punch] = InputState{true, 0.f};
    input.actions[InputAction::Kick] = InputState{true, 0.f};
    input.actions[InputAction::Defend] = InputState{true, 0.f};
    input.actions[InputAction::Jump] = InputState{true, 0.f};

    AnalogInputComponent analog;
    analog.moveX = 1.f;

    StateComponent state;
    state.current = StateId::Idle;

    VelocityComponent velocity;

    GroundedComponent grounded;
    grounded.onGround = true;

    auto& components = this->scene.world().components();
    components.add<InputComponent>(entity, input);
    components.add<AnalogInputComponent>(entity, analog);
    components.add<StateComponent>(entity, state);
    components.add<VelocityComponent>(entity, velocity);
    components.add<PlayerComponent>(entity, PlayerComponent{1});
    components.add<GroundedComponent>(entity, grounded);

    std::vector<TriggerEvent> events;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent& event) { events.push_back(event); });

    this->scene.systems().addSystem<PlayerControlSystem>(this->bus, 240.f, -420.f);
    this->scene.update(0.016f);

    const auto& updatedVelocity = components.get<VelocityComponent>(entity);
    REQUIRE(updatedVelocity.velocity.x == Catch::Approx(240.f));
    REQUIRE(updatedVelocity.velocity.y == Catch::Approx(-420.f));

    REQUIRE(events.size() == 3);
    REQUIRE(events[0].trigger == TriggerId::Punched);
    REQUIRE(events[1].trigger == TriggerId::Kicked);
    REQUIRE(events[2].trigger == TriggerId::Blocked);
}

TEST_CASE_METHOD(PlayerControlSystemFixture, "PlayerControlSystem blocks movement and combat triggers in non-movable states",
    "[integration][player_control_system]"
) {
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Punch] = InputState{true, 0.f};
    input.actions[InputAction::Kick] = InputState{true, 0.f};
    input.actions[InputAction::Defend] = InputState{true, 0.f};

    AnalogInputComponent analog;
    analog.moveX = 1.f;

    StateComponent state;
    state.current = StateId::BeingHit;

    VelocityComponent velocity;
    velocity.velocity.x = 123.f;

    auto& components = this->scene.world().components();
    components.add<InputComponent>(entity, input);
    components.add<AnalogInputComponent>(entity, analog);
    components.add<StateComponent>(entity, state);
    components.add<VelocityComponent>(entity, velocity);
    components.add<PlayerComponent>(entity, PlayerComponent{2});

    int triggerCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++triggerCount; });

    this->scene.systems().addSystem<PlayerControlSystem>(this->bus, 240.f, -420.f);
    this->scene.update(0.016f);

    const auto& updatedVelocity = this->scene.world().components().get<VelocityComponent>(entity);
    REQUIRE(updatedVelocity.velocity.x == Catch::Approx(0.f));
    REQUIRE(triggerCount == 0);
}

TEST_CASE_METHOD(PlayerControlSystemFixture, "PlayerControlSystem blocks movement and triggers when frozen by hitstop",
    "[integration][player_control_system]"
) {
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Punch] = InputState{true, 0.f};
    input.actions[InputAction::MoveRight] = InputState{true, 0.f};

    AnalogInputComponent analog;
    analog.moveX = 1.f;

    StateComponent state;
    state.current = StateId::Idle;

    VelocityComponent velocity{ 50.f, 0.f };
    HitstopComponent hitstop{ .remaining = 1.f, .frozen = true };

    auto& components = this->scene.world().components();
    components.add<InputComponent>(entity, input);
    components.add<AnalogInputComponent>(entity, analog);
    components.add<StateComponent>(entity, state);
    components.add<VelocityComponent>(entity, velocity);
    components.add<PlayerComponent>(entity, PlayerComponent{1});
    components.add<HitstopComponent>(entity, hitstop);

    int triggerCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++triggerCount; });

    this->scene.systems().addSystem<PlayerControlSystem>(this->bus, 240.f, -420.f);
    this->scene.update(0.016f);

    const auto& updatedVelocity = components.get<VelocityComponent>(entity);
    REQUIRE(updatedVelocity.velocity.x == Catch::Approx(50.f));
    REQUIRE(triggerCount == 0);
}
