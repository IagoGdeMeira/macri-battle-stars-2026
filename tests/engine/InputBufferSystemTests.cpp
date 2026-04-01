#include "../../src/engine/include/InputBufferSystem/InputBufferSystem.h"

#include "../../src/domain/components/InputBufferComponent.h"
#include "../../src/domain/components/PlayerComponent.h"

#include "../../src/engine/events/KeyEvent.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/Scene/Scene.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("InputBufferSystem pushes mapped pressed key to matching player",
    "[integration][input_buffer_system]"
) {
    EventBus bus;
    Scene scene(bus);

    scene.world().components().registerComponent<InputBufferComponent>();
    scene.world().components().registerComponent<PlayerComponent>();

    const auto entity = scene.world().entities().create();

    scene.world().components().add<InputBufferComponent>(entity, InputBufferComponent{});
    scene.world().components().add<PlayerComponent>(entity, PlayerComponent { 1 });

    InputContext context;
    context.bindings[1].keyMap[KeyCode::A] = InputAction::Attack;

    scene.systems().addSystem<InputBufferSystem>(bus, context);

    bus.emit<KeyEvent>(KeyCode::A, true, 1);
    scene.update(0.016f);

    const auto& updated = scene.world().components().get<InputBufferComponent>(entity);
    REQUIRE(updated.buffer.size() == 1);
    REQUIRE(updated.buffer.back().action == InputAction::Attack);
    REQUIRE(updated.buffer.back().time == 0.0f);
}

TEST_CASE("InputBufferSystem ignores key release events",
    "[integration][input_buffer_system]"
) {
    EventBus bus;
    Scene scene(bus);

    scene.world().components().registerComponent<InputBufferComponent>();
    scene.world().components().registerComponent<PlayerComponent>();

    const auto entity = scene.world().entities().create();

    scene.world().components().add<InputBufferComponent>(entity, InputBufferComponent{});
    scene.world().components().add<PlayerComponent>(entity, PlayerComponent { 1 });

    InputContext context;
    context.bindings[1].keyMap[KeyCode::A] = InputAction::Attack;

    scene.systems().addSystem<InputBufferSystem>(bus, context);

    bus.emit<KeyEvent>(KeyCode::A, false, 1);
    scene.update(0.016f);

    const auto& updated = scene.world().components().get<InputBufferComponent>(entity);
    REQUIRE(updated.buffer.empty());
}

TEST_CASE("InputBufferSystem increments time and drops expired inputs",
    "[integration][input_buffer_system]"
) {
    EventBus bus;
    Scene scene(bus);

    scene.world().components().registerComponent<InputBufferComponent>();
    scene.world().components().registerComponent<PlayerComponent>();

    const auto entity = scene.world().entities().create();

    InputBufferComponent buffer;
    buffer.buffer.push_back({ InputAction::MoveLeft, 0.49f });
    buffer.buffer.push_back({ InputAction::MoveRight, 0.10f });

    scene.world().components().add<InputBufferComponent>(entity, buffer);
    scene.world().components().add<PlayerComponent>(entity, PlayerComponent { 2 });

    InputContext context;
    scene.systems().addSystem<InputBufferSystem>(bus, context);

    scene.update(0.02f);

    const auto& updated = scene.world().components().get<InputBufferComponent>(entity);
    REQUIRE(updated.buffer.size() == 1);
    REQUIRE(updated.buffer.front().action == InputAction::MoveRight);
    REQUIRE(updated.buffer.front().time == Catch::Approx(0.12f));
}
