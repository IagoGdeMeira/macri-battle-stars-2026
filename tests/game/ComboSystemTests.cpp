#include "../../src/game/include/ComboSystem/ComboSystem.h"

#include "../../src/domain/components/InputBufferComponent.h"

#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/Scene/Scene.h"

#include "../../src/game/events/ComboExecutedEvent.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("ComboSystem emits best matching combo by priority",
    "[integration][combo_system]"
) {
    EventBus bus;
    Scene scene(bus);

    scene.world().components().registerComponent<InputBufferComponent>();

    const auto entity = scene.world().entities().create();

    InputBufferComponent buffer;
    buffer.buffer.push_back({ InputAction::Attack, 0.05f });
    buffer.buffer.push_back({ InputAction::Jump, 0.05f });

    scene.world().components().add<InputBufferComponent>(entity, buffer);

    Combo low;
    low.name = "low";
    low.priority = 1;
    low.steps =
    {
        { InputAction::Attack, 200 },
        { InputAction::Jump, 200 }
    };

    Combo high;
    high.name = "high";
    high.priority = 10;
    high.steps =
    {
        { InputAction::Attack, 200 },
        { InputAction::Jump, 200 }
    };

    std::vector<ComboExecutedEvent> emitted;
    bus.subscribe<ComboExecutedEvent>([&](const ComboExecutedEvent& event)
    { emitted.push_back(event); });

    scene.systems().addSystem<ComboSystem>(bus, std::vector<Combo> { low, high });

    scene.update(0.016f);

    REQUIRE(emitted.size() == 1);
    REQUIRE(emitted[0].entity == entity);
    REQUIRE(emitted[0].name == "high");
}

TEST_CASE("ComboSystem clears buffer when combo consumes input",
    "[integration][combo_system]"
) {
    EventBus bus;
    Scene scene(bus);

    scene.world().components().registerComponent<InputBufferComponent>();

    const auto entity = scene.world().entities().create();

    InputBufferComponent buffer;
    buffer.buffer.push_back({ InputAction::Attack, 0.01f });

    scene.world().components().add<InputBufferComponent>(entity, buffer);

    Combo combo;
    combo.name = "single";
    combo.consumeInput = true;
    combo.steps = { { InputAction::Attack, 100 } };

    scene.systems().addSystem<ComboSystem>(bus, std::vector<Combo> { combo });

    scene.update(0.016f);

    const auto& updated = scene.world().components().get<InputBufferComponent>(entity);
    REQUIRE(updated.buffer.empty());
}

TEST_CASE("ComboSystem keeps buffer when combo does not consume input",
    "[integration][combo_system]"
) {
    EventBus bus;
    Scene scene(bus);

    scene.world().components().registerComponent<InputBufferComponent>();

    const auto entity = scene.world().entities().create();

    InputBufferComponent buffer;
    buffer.buffer.push_back({ InputAction::Defend, 0.01f });

    scene.world().components().add<InputBufferComponent>(entity, buffer);

    Combo combo;
    combo.name = "hold";
    combo.consumeInput = false;
    combo.steps = { { InputAction::Defend, 100 } };

    scene.systems().addSystem<ComboSystem>(bus, std::vector<Combo> { combo });

    scene.update(0.016f);

    const auto& updated = scene.world().components().get<InputBufferComponent>(entity);
    REQUIRE(updated.buffer.size() == 1);
    REQUIRE(updated.buffer.front().action == InputAction::Defend);
}
