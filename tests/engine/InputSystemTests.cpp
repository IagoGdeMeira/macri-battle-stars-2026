#include "../../src/engine/include/InputSystem/InputSystem.h"

#include "../../src/domain/components/InputComponent.h"
#include "../../src/domain/components/PlayerComponent.h"

#include "../../src/engine/events/KeyEvent.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/Scene/Scene.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("InputSystem updates mapped action for matching player",
    "[integration][input_system]"
) {
    EventBus bus;
    Scene scene(bus);

    scene.world().components().registerComponent<InputComponent>();
    scene.world().components().registerComponent<PlayerComponent>();

    const auto entity = scene.world().entities().create();
    scene.world().components().add<InputComponent>(entity, InputComponent{});
    scene.world().components().add<PlayerComponent>(entity, PlayerComponent{1});

    InputContext context;
    context.bindings[1].keyMap[KeyCode::A] = InputAction::Attack;

    scene.systems().addSystem<InputSystem>(bus, context);

    bus.emit<KeyEvent>(KeyCode::A, true, 1);
    scene.update(0.016f);

    const auto& input = scene.world().components().get<InputComponent>(entity);
    REQUIRE(input.actions.at(InputAction::Attack).pressed == true);
}

TEST_CASE("InputSystem resets heldTime when mapped key is released",
    "[integration][input_system]"
) {
    EventBus bus;
    Scene scene(bus);

    scene.world().components().registerComponent<InputComponent>();
    scene.world().components().registerComponent<PlayerComponent>();

    const auto entity = scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::MoveLeft] = InputState{true, 2.5f};

    scene.world().components().add<InputComponent>(entity, input);
    scene.world().components().add<PlayerComponent>(entity, PlayerComponent{7});

    InputContext context;
    context.bindings[7].keyMap[KeyCode::Left] = InputAction::MoveLeft;

    scene.systems().addSystem<InputSystem>(bus, context);

    bus.emit<KeyEvent>(KeyCode::Left, false, 7);
    scene.update(0.016f);

    const auto& updated = scene.world().components().get<InputComponent>(entity);
    REQUIRE(updated.actions.at(InputAction::MoveLeft).pressed == false);
    REQUIRE(updated.actions.at(InputAction::MoveLeft).heldTime == 0.0f);
}

TEST_CASE("InputSystem ignores events from other players",
    "[integration][input_system]"
) {
    EventBus bus;
    Scene scene(bus);

    scene.world().components().registerComponent<InputComponent>();
    scene.world().components().registerComponent<PlayerComponent>();

    const auto entity = scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Defend] = InputState{false, 0.0f};

    scene.world().components().add<InputComponent>(entity, input);
    scene.world().components().add<PlayerComponent>(entity, PlayerComponent{3});

    InputContext context;
    context.bindings[3].keyMap[KeyCode::RShift] = InputAction::Defend;

    scene.systems().addSystem<InputSystem>(bus, context);

    bus.emit<KeyEvent>(KeyCode::RShift, true, 99);
    scene.update(0.016f);

    const auto& updated = scene.world().components().get<InputComponent>(entity);
    REQUIRE(updated.actions.at(InputAction::Defend).pressed == false);
}
