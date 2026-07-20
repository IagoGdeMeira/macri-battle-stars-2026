#include "game/include/HorizontalMovementSystem/HorizontalMovementSystem.h"

#include "domain/components/AnalogInputComponent.h"
#include "domain/components/HitstopComponent.h"
#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/components/StateComponent.h"
#include "domain/components/VelocityComponent.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/Scene/Scene.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class HorizontalMovementSystemFixture
{
public:
    EventBus bus;
    Scene scene{this->bus};

    static constexpr float UPDATE_DELAY = 0.016f;

    HorizontalMovementSystemFixture()
    {
        auto& comp = this->scene.world().components();
        comp.registerComponent<AnalogInputComponent>();
        comp.registerComponent<HitstopComponent>();
        comp.registerComponent<InputComponent>();
        comp.registerComponent<PlayerComponent>();
        comp.registerComponent<StateComponent>();
        comp.registerComponent<VelocityComponent>();
    }
};

TEST_CASE_METHOD(HorizontalMovementSystemFixture, "HorizontalMovementSystem moves left and right using digital input",
    "[integration][horizontal_movement_system]"
) {
    const auto& updateDelay = HorizontalMovementSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();
    
    InputComponent input;
    input.actions[InputAction::MoveLeft] = InputComponent::State{true, 0.f};

    AnalogInputComponent analog;
    StateComponent state;
    state.current = StateId::Idle;

    VelocityComponent velocity;

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<AnalogInputComponent>(entity, analog);
    comp.add<StateComponent>(entity, state);
    comp.add<VelocityComponent>(entity, velocity);
    comp.add<PlayerComponent>(entity, PlayerComponent{1});

    this->scene.systems().addSystem<HorizontalMovementSystem>(300.f);
    this->scene.update(updateDelay);

    auto& updatedVelocity = comp.get<VelocityComponent>(entity);
    REQUIRE(updatedVelocity.velocity.x == Catch::Approx(-300.f));

    auto& storedInput = comp.get<InputComponent>(entity);
    storedInput.actions.clear();
    storedInput.actions[InputAction::MoveRight] = InputComponent::State{true, 0.f};
    this->scene.update(updateDelay);

    REQUIRE(updatedVelocity.velocity.x == Catch::Approx(300.f));
}

TEST_CASE_METHOD(HorizontalMovementSystemFixture, "HorizontalMovementSystem prefers analog movement over digital input",
    "[integration][horizontal_movement_system]"
) {
    const auto& updateDelay = HorizontalMovementSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::MoveLeft] = InputComponent::State{true, 0.f};

    AnalogInputComponent analog;
    analog.move.x = 0.5f;

    StateComponent state;
    state.current = StateId::Idle;

    VelocityComponent velocity;

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<AnalogInputComponent>(entity, analog);
    comp.add<StateComponent>(entity, state);
    comp.add<VelocityComponent>(entity, velocity);
    comp.add<PlayerComponent>(entity, PlayerComponent{2});

    this->scene.systems().addSystem<HorizontalMovementSystem>(200.f);
    this->scene.update(updateDelay);

    const auto& updatedVelocity = comp.get<VelocityComponent>(entity);
    REQUIRE(updatedVelocity.velocity.x == Catch::Approx(100.f));
}

TEST_CASE_METHOD(HorizontalMovementSystemFixture, "HorizontalMovementSystem blocks movement when state cannot move",
    "[integration][horizontal_movement_system]"
) {
    const auto& updateDelay = HorizontalMovementSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::MoveRight] = InputComponent::State{true, 0.f};

    AnalogInputComponent analog;
    analog.move.x = 1.f;

    StateComponent state;
    state.current = StateId::BeingHit;

    VelocityComponent velocity;
    velocity.velocity.x = 123.f;

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<AnalogInputComponent>(entity, analog);
    comp.add<StateComponent>(entity, state);
    comp.add<VelocityComponent>(entity, velocity);
    comp.add<PlayerComponent>(entity, PlayerComponent{3});

    this->scene.systems().addSystem<HorizontalMovementSystem>(240.f);
    this->scene.update(updateDelay);

    const auto& updatedVelocity = comp.get<VelocityComponent>(entity);
    REQUIRE(updatedVelocity.velocity.x == Catch::Approx(0.f));
}

TEST_CASE_METHOD(HorizontalMovementSystemFixture, "HorizontalMovementSystem keeps velocity unchanged while frozen by hitstop",
    "[integration][horizontal_movement_system]"
) {
    const auto& updateDelay = HorizontalMovementSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::MoveLeft] = InputComponent::State{true, 0.f};

    AnalogInputComponent analog;
    analog.move.x = -1.f;

    StateComponent state;
    state.current = StateId::Idle;

    VelocityComponent velocity;
    velocity.velocity.x = 42.f;

    HitstopComponent hitstop{ .remaining = 1.f, .frozen = true };

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<AnalogInputComponent>(entity, analog);
    comp.add<StateComponent>(entity, state);
    comp.add<VelocityComponent>(entity, velocity);
    comp.add<PlayerComponent>(entity, PlayerComponent{4});
    comp.add<HitstopComponent>(entity, hitstop);

    this->scene.systems().addSystem<HorizontalMovementSystem>(240.f);
    this->scene.update(updateDelay);

    const auto& updatedVelocity = comp.get<VelocityComponent>(entity);
    REQUIRE(updatedVelocity.velocity.x == Catch::Approx(42.f));
}
