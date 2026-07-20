#include "game/include/TriggerGenerationSystem/TriggerGenerationSystem.h"

#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/Scene/Scene.h"

#include "game/events/TriggerEvent.h"

#include <catch2/catch_test_macros.hpp>
#include <vector>

class TriggerGenerationSystemFixture
{
public:
    EventBus bus;
    Scene scene{this->bus};

    TriggerGenerationSystemFixture()
    {
        auto& comp = this->scene.world().components();
        comp.registerComponent<InputComponent>();
        comp.registerComponent<PlayerComponent>();
    }
};

TEST_CASE_METHOD(TriggerGenerationSystemFixture, "TriggerGenerationSystem emits one event per mapped trigger",
    "[integration][trigger_generation_system]"
) {
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Punch] = InputComponent::State{true, 0.f};

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<PlayerComponent>(entity, PlayerComponent{1});

    TriggerContext context;
    context.bindings[InputAction::Punch] = {TriggerId::Jumped, TriggerId::Punched};

    std::vector<TriggerEvent> events;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent& event) { events.push_back(event); });
    this->scene.systems().addSystem<TriggerGenerationSystem>(this->bus, context);
    this->scene.update(0.016f);

    REQUIRE(events.size() == 2);
    REQUIRE(events[0].entity == entity);
    REQUIRE(events[0].trigger == TriggerId::Jumped);
    REQUIRE(events[1].entity == entity);
    REQUIRE(events[1].trigger == TriggerId::Punched);
}

TEST_CASE_METHOD(TriggerGenerationSystemFixture, "TriggerGenerationSystem emits kick triggers for pressed kick input",
    "[integration][trigger_generation_system]"
) {
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Kick] = InputComponent::State{true, 0.f};

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<PlayerComponent>(entity, PlayerComponent{2});

    TriggerContext context;
    context.bindings[InputAction::Kick] = {TriggerId::Kicked};

    std::vector<TriggerEvent> events;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent& event) { events.push_back(event); });

    this->scene.systems().addSystem<TriggerGenerationSystem>(this->bus, context);
    this->scene.update(0.016f);

    REQUIRE(events.size() == 1);
    REQUIRE(events[0].entity == entity);
    REQUIRE(events[0].trigger == TriggerId::Kicked);
}

TEST_CASE_METHOD(TriggerGenerationSystemFixture, "TriggerGenerationSystem ignores unpressed and unmapped actions",
    "[integration][trigger_generation_system]"
) {
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Kick] = InputComponent::State{false, 0.f};
    input.actions[InputAction::Jump] = InputComponent::State{true, 0.f};

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<PlayerComponent>(entity, PlayerComponent{2});

    TriggerContext context;
    context.bindings[InputAction::Kick] = {TriggerId::Blocked};

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++eventCount; });

    this->scene.systems().addSystem<TriggerGenerationSystem>(this->bus, context);
    this->scene.update(0.016f);

    REQUIRE(eventCount == 0);
}
