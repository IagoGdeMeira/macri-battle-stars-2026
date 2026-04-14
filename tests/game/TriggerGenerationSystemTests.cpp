#include "../../src/game/include/TriggerGenerationSystem/TriggerGenerationSystem.h"

#include "../../src/domain/components/InputComponent.h"
#include "../../src/domain/components/PlayerComponent.h"
#include "../../src/game/events/TriggerEvent.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/Scene/Scene.h"

#include <catch2/catch_test_macros.hpp>

#include <vector>

class TriggerGenerationSystemFixture
{
public:
    TriggerGenerationSystemFixture()
    {
        auto& components = this->scene.world().components();

        components.registerComponent<InputComponent>();
        components.registerComponent<PlayerComponent>();
    }

    EventBus bus;
    Scene scene{bus};
};

TEST_CASE_METHOD(TriggerGenerationSystemFixture,
    "TriggerGenerationSystem emits one event per mapped trigger",
    "[integration][trigger_generation_system]"
) {
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Attack] = InputState{true, 0.0f};

    this->scene.world().components().add<InputComponent>(entity, input);
    this->scene.world().components().add<PlayerComponent>(entity, PlayerComponent{1});

    TriggerContext context;
    context.bindings[InputAction::Attack] = {TriggerId::Jumped, TriggerId::Punched};

    std::vector<TriggerEvent> events;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent& event)
    { events.push_back(event); });

    this->scene.systems().addSystem<TriggerGenerationSystem>(this->bus, context);

    this->scene.update(0.016f);

    REQUIRE(events.size() == 2);
    REQUIRE(events[0].entity == entity);
    REQUIRE(events[0].trigger == TriggerId::Jumped);
    REQUIRE(events[1].entity == entity);
    REQUIRE(events[1].trigger == TriggerId::Punched);
}

TEST_CASE_METHOD(TriggerGenerationSystemFixture,
    "TriggerGenerationSystem ignores unpressed and unmapped actions",
    "[integration][trigger_generation_system]"
) {
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Attack] = InputState{false, 0.0f};
    input.actions[InputAction::Jump] = InputState{true, 0.0f};

    this->scene.world().components().add<InputComponent>(entity, input);
    this->scene.world().components().add<PlayerComponent>(entity, PlayerComponent{2});

    TriggerContext context;
    context.bindings[InputAction::Attack] = {TriggerId::Blocked};

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&)
    { ++eventCount; });

    this->scene.systems().addSystem<TriggerGenerationSystem>(this->bus, context);

    this->scene.update(0.016f);

    REQUIRE(eventCount == 0);
}
