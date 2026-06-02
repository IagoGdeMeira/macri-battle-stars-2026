#include "../src/engine/include/InputSystem/InputSystem.h"

#include "../src/domain/components/AnalogInputComponent.h"
#include "../src/domain/components/InputComponent.h"
#include "../src/domain/components/PlayerComponent.h"
#include "../src/domain/include/InputAction/InputAction.h"

#include "../src/engine/events/InputEvent.h"
#include "../src/engine/include/EventBus/EventBus.h"
#include "../src/engine/include/InputSource/InputSource.h"
#include "../src/engine/include/Scene/Scene.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class InputSystemFixture
{
public:
    InputSystemFixture()
    {
        auto& components = this->scene.world().components();
        components.registerComponent<InputComponent>();
        components.registerComponent<PlayerComponent>();
        components.registerComponent<AnalogInputComponent>();
    }

    EventBus bus;
    Scene scene{bus};
};

TEST_CASE_METHOD(InputSystemFixture, "InputSystem updates mapped action for matching player",
    "[integration][input_system]"
) {
    const auto entity = this->scene.world().entities().create();
    auto& components = this->scene.world().components();
    components.add<InputComponent>(entity, InputComponent{});
    components.add<PlayerComponent>(entity, PlayerComponent{1});

    InputContext context;
    context.bindings[1].keyMap[InputSource::keyboard(KeyCode::A)] = InputAction::Punch;

    this->scene.systems().addSystem<InputSystem>(this->bus, context);

    this->bus.emit<DigitalInputEvent>(InputSource::keyboard(KeyCode::A), 1, true);
    this->scene.update(0.016f);

    const auto& input = this->scene.world().components().get<InputComponent>(entity);
    REQUIRE(input.actions.at(InputAction::Punch).pressed == true);
}

TEST_CASE_METHOD(InputSystemFixture, "InputSystem keeps heldTime progression when mapped key is released",
    "[integration][input_system]"
) {
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::MoveLeft] = InputState{true, 2.5f};

    auto& components = this->scene.world().components();
    components.add<InputComponent>(entity, input);
    components.add<PlayerComponent>(entity, PlayerComponent{7});

    InputContext context;
    context.bindings[7].keyMap[InputSource::keyboard(KeyCode::Left)] = InputAction::MoveLeft;

    this->scene.systems().addSystem<InputSystem>(this->bus, context);

    this->bus.emit<DigitalInputEvent>(InputSource::keyboard(KeyCode::Left), 7, false);
    this->scene.update(0.016f);

    const auto& updated = this->scene.world().components().get<InputComponent>(entity);
    REQUIRE(updated.actions.at(InputAction::MoveLeft).pressed == false);
    REQUIRE(updated.actions.at(InputAction::MoveLeft).heldTime == Catch::Approx(2.516f));
}

TEST_CASE_METHOD(InputSystemFixture, "InputSystem ignores events from other players",
    "[integration][input_system]"
) {
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Defend] = InputState{false, 0.0f};

    auto& components = this->scene.world().components();
    components.add<InputComponent>(entity, input);
    components.add<PlayerComponent>(entity, PlayerComponent{3});

    InputContext context;
    context.bindings[3].keyMap[InputSource::keyboard(KeyCode::RShift)] = InputAction::Defend;

    this->scene.systems().addSystem<InputSystem>(this->bus, context);

    this->bus.emit<DigitalInputEvent>(InputSource::keyboard(KeyCode::RShift), 99, true);
    this->scene.update(0.016f);

    const auto& updated = this->scene.world().components().get<InputComponent>(entity);
    REQUIRE(updated.actions.at(InputAction::Defend).pressed == false);
}
