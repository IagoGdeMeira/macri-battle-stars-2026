#include "engine/include/InputSystem/InputSystem.h"

#include "domain/components/AnalogInputComponent.h"
#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/value_objects/InputAction/InputAction.h"

#include "engine/events/InputEvent.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/include/InputSource/InputSource.h"
#include "engine/include/Scene/Scene.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class InputSystemFixture
{
public:
    EventBus bus;
    Scene scene{this->bus};

    InputSystemFixture()
    {
        auto& comp = this->scene.world().components();
        comp.registerComponent<InputComponent>();
        comp.registerComponent<PlayerComponent>();
        comp.registerComponent<AnalogInputComponent>();
    }
};

TEST_CASE_METHOD(InputSystemFixture, "InputSystem updates mapped action for matching player",
    "[integration][input_system]"
) {
    const auto entity = this->scene.world().entities().create();
    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, InputComponent{});
    comp.add<PlayerComponent>(entity, PlayerComponent{1});

    InputContext context;
    context.bindings[1].keyMap[InputSource::keyboard(KeyCode::A)] = InputAction::Punch;

    this->scene.systems().addSystem<InputSystem>(this->bus, context);

    this->bus.emit<DigitalInputEvent>(InputSource::keyboard(KeyCode::A), 1, true);
    this->scene.update(0.016f);

    const auto& input = comp.get<InputComponent>(entity);
    REQUIRE(input.actions.at(InputAction::Punch).pressed == true);
}

TEST_CASE_METHOD(InputSystemFixture, "InputSystem keeps heldTime unchanged when mapped key is released",
    "[integration][input_system]"
) {
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::MoveLeft] = InputComponent::State{true, 2.5f};

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<PlayerComponent>(entity, PlayerComponent{7});

    InputContext context;
    context.bindings[7].keyMap[InputSource::keyboard(KeyCode::Left)] = InputAction::MoveLeft;

    this->scene.systems().addSystem<InputSystem>(this->bus, context);

    this->bus.emit<DigitalInputEvent>(InputSource::keyboard(KeyCode::Left), 7, false);
    this->scene.update(0.016f);

    const auto& updated = comp.get<InputComponent>(entity);
    REQUIRE(updated.actions.at(InputAction::MoveLeft).pressed == false);
    REQUIRE(updated.actions.at(InputAction::MoveLeft).heldTime == Catch::Approx(2.5f));
}

TEST_CASE_METHOD(InputSystemFixture, "InputSystem ignores events from other players",
    "[integration][input_system]"
) {
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Defend] = InputComponent::State{false, 0.f};

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<PlayerComponent>(entity, PlayerComponent{3});

    InputContext context;
    context.bindings[3].keyMap[InputSource::keyboard(KeyCode::RShift)] = InputAction::Defend;

    this->scene.systems().addSystem<InputSystem>(this->bus, context);

    this->bus.emit<DigitalInputEvent>(InputSource::keyboard(KeyCode::RShift), 99, true);
    this->scene.update(0.016f);

    const auto& updated = comp.get<InputComponent>(entity);
    REQUIRE(updated.actions.at(InputAction::Defend).pressed == false);
}

TEST_CASE_METHOD(InputSystemFixture, "InputSystem keeps action pressed when at least one source is active",
    "[integration][input_system]"
) {
    const auto entity = this->scene.world().entities().create();
    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, InputComponent{});
    comp.add<PlayerComponent>(entity, PlayerComponent{0});

    InputContext context;
    context.bindings[0].keyMap[InputSource::keyboard(KeyCode::D)] = InputAction::MoveRight;
    context.bindings[0].keyMap[InputSource::gamepad(GamepadButton::DpadRight)] = InputAction::MoveRight;

    this->scene.systems().addSystem<InputSystem>(this->bus, context);

    this->bus.emit<DigitalInputEvent>(InputSource::keyboard(KeyCode::D), 0, true);
    this->bus.emit<DigitalInputEvent>(InputSource::gamepad(GamepadButton::DpadRight), 0, false);
    this->scene.update(0.016f);

    const auto& input = comp.get<InputComponent>(entity);
    REQUIRE(input.actions.at(InputAction::MoveRight).pressed == true);
}