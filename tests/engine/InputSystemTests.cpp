#include "../../src/engine/include/InputSystem/InputSystem.h"

#include "../../src/domain/components/InputComponent.h"
#include "../../src/domain/components/PlayerComponent.h"

#include "../../src/engine/events/KeyEvent.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/Scene/Scene.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class InputSystemFixture
{
public:
    InputSystemFixture()
    {
        this->scene.world().components().registerComponent<InputComponent>();
        this->scene.world().components().registerComponent<PlayerComponent>();
    }

    EventBus bus;
    Scene scene{bus};
};

TEST_CASE_METHOD(InputSystemFixture, "InputSystem updates mapped action for matching player",
    "[integration][input_system]"
) {
    const auto entity = this->scene.world().entities().create();
    this->scene.world().components().add<InputComponent>(entity, InputComponent{});
    this->scene.world().components().add<PlayerComponent>(entity, PlayerComponent{1});

    InputContext context;
    context.bindings[1].keyMap[KeyCode::A] = InputAction::Attack;

    this->scene.systems().addSystem<InputSystem>(bus, context);

    bus.emit<KeyEvent>(KeyCode::A, true, 1);
    this->scene.update(0.016f);

    const auto& input = this->scene.world().components().get<InputComponent>(entity);
    REQUIRE(input.actions.at(InputAction::Attack).pressed == true);
}

TEST_CASE_METHOD(InputSystemFixture, "InputSystem keeps heldTime progression when mapped key is released",
    "[integration][input_system]"
) {
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::MoveLeft] = InputState{true, 2.5f};

    this->scene.world().components().add<InputComponent>(entity, input);
    this->scene.world().components().add<PlayerComponent>(entity, PlayerComponent{7});

    InputContext context;
    context.bindings[7].keyMap[KeyCode::Left] = InputAction::MoveLeft;

    this->scene.systems().addSystem<InputSystem>(this->bus, context);

    this->bus.emit<KeyEvent>(KeyCode::Left, false, 7);
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

    this->scene.world().components().add<InputComponent>(entity, input);
    this->scene.world().components().add<PlayerComponent>(entity, PlayerComponent{3});

    InputContext context;
    context.bindings[3].keyMap[KeyCode::RShift] = InputAction::Defend;

    this->scene.systems().addSystem<InputSystem>(this->bus, context);

    this->bus.emit<KeyEvent>(KeyCode::RShift, true, 99);
    this->scene.update(0.016f);

    const auto& updated = this->scene.world().components().get<InputComponent>(entity);
    REQUIRE(updated.actions.at(InputAction::Defend).pressed == false);
}
