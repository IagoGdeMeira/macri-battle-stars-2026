#include "game/include/DirectionTriggerSystem/DirectionTriggerSystem.h"

#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/include/Scene/Scene.h"
#include "game/events/TriggerEvent.h"

#include <catch2/catch_test_macros.hpp>
#include <vector>

class DirectionTriggerSystemFixture
{
public:
    EventBus bus;
    Scene scene{this->bus};

    static constexpr float UPDATE_DELAY = 0.016f;

    DirectionTriggerSystemFixture()
    {
        auto& comp = this->scene.world().components();
        comp.registerComponent<InputComponent>();
        comp.registerComponent<PlayerComponent>();
    }
};

TEST_CASE_METHOD(DirectionTriggerSystemFixture, "DirectionTriggerSystem emits press and release triggers for left input",
    "[integration][direction_trigger_system]"
) {
    const auto& updateDelay = DirectionTriggerSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<PlayerComponent>(entity, PlayerComponent{1});

    std::vector<TriggerEvent> events;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent& event) { events.push_back(event); });

    this->scene.systems().addSystem<DirectionTriggerSystem>(this->bus);

    InputComponent& storedInput = comp.get<InputComponent>(entity);
    storedInput.actions[InputAction::MoveLeft] = InputComponent::State{true, 0.f};
    this->scene.update(updateDelay);

    storedInput.actions[InputAction::MoveLeft] = InputComponent::State{false, 0.f};
    this->scene.update(updateDelay);

    REQUIRE(events.size() == 2);
    REQUIRE(events[0].entity == entity);
    REQUIRE(events[0].trigger == TriggerId::MoveLeft);
    REQUIRE(events[1].entity == entity);
    REQUIRE(events[1].trigger == TriggerId::MoveLeftReleased);
}

TEST_CASE_METHOD(DirectionTriggerSystemFixture, "DirectionTriggerSystem emits press and release triggers for right input",
    "[integration][direction_trigger_system]"
) {
    const auto& updateDelay = DirectionTriggerSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<PlayerComponent>(entity, PlayerComponent{2});

    std::vector<TriggerEvent> events;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent& event) { events.push_back(event); });

    this->scene.systems().addSystem<DirectionTriggerSystem>(this->bus);

    InputComponent& storedInput = comp.get<InputComponent>(entity);
    storedInput.actions[InputAction::MoveRight] = InputComponent::State{true, 0.f};
    this->scene.update(updateDelay);

    storedInput.actions[InputAction::MoveRight] = InputComponent::State{false, 0.f};
    this->scene.update(updateDelay);

    REQUIRE(events.size() == 2);
    REQUIRE(events[0].entity == entity);
    REQUIRE(events[0].trigger == TriggerId::MoveRight);
    REQUIRE(events[1].entity == entity);
    REQUIRE(events[1].trigger == TriggerId::MoveRightReleased);
}

TEST_CASE_METHOD(DirectionTriggerSystemFixture, "DirectionTriggerSystem does not repeat direction triggers while held",
    "[integration][direction_trigger_system]"
) {
    const auto& updateDelay = DirectionTriggerSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::MoveLeft] = InputComponent::State{true, 0.f};

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<PlayerComponent>(entity, PlayerComponent{3});

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++eventCount; });

    this->scene.systems().addSystem<DirectionTriggerSystem>(this->bus);
    this->scene.update(updateDelay);
    this->scene.update(updateDelay);

    REQUIRE(eventCount == 1);
}
