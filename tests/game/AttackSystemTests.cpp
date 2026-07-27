#include "game/include/AttackSystem/AttackSystem.h"

#include "domain/components/HitstopComponent.h"
#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/components/StateComponent.h"
#include "domain/value_objects/InputAction/InputAction.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/Scene/Scene.h"

#include "game/events/TriggerEvent.h"

#include <catch2/catch_test_macros.hpp>
#include <vector>

class AttackSystemFixture
{
public:
    EventBus bus;
    Scene scene{this->bus};

    static constexpr float UPDATE_DELAY = 0.016f;

    AttackSystemFixture()
    {
        auto& comp = this->scene.world().components();
        comp.registerComponent<HitstopComponent>();
        comp.registerComponent<InputComponent>();
        comp.registerComponent<PlayerComponent>();
        comp.registerComponent<StateComponent>();
    }
};

TEST_CASE_METHOD(AttackSystemFixture, "AttackSystem emits the configured trigger for each attack input",
    "[integration][attack_system]"
) {
    const auto& updateDelay = AttackSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Punch] = InputComponent::State{true, 0.f};
    input.actions[InputAction::Kick] = InputComponent::State{true, 0.f};
    input.actions[InputAction::Defend] = InputComponent::State{true, 0.f};

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<PlayerComponent>(entity, PlayerComponent{1});
    comp.add<StateComponent>(entity, StateComponent{StateId::Idle});

    std::vector<TriggerEvent> events;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent& event) { events.push_back(event); });

    auto& systems = this->scene.systems();
    systems.addSystem<AttackSystem>(this->bus, InputAction::Punch, TriggerId::Punched);
    systems.addSystem<AttackSystem>(this->bus, InputAction::Kick, TriggerId::Kicked);
    systems.addSystem<AttackSystem>(this->bus, InputAction::Defend, TriggerId::Blocked);
    this->scene.update(updateDelay);

    REQUIRE(events.size() == 3);
    REQUIRE(events[0].entity == entity);
    REQUIRE(events[0].trigger == TriggerId::Punched);
    REQUIRE(events[1].entity == entity);
    REQUIRE(events[1].trigger == TriggerId::Kicked);
    REQUIRE(events[2].entity == entity);
    REQUIRE(events[2].trigger == TriggerId::Blocked);
}

TEST_CASE_METHOD(AttackSystemFixture, "AttackSystem blocks attacks in non-attackable states",
    "[integration][attack_system]"
) {
    const auto& updateDelay = AttackSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Punch] = InputComponent::State{true, 0.f};

    StateComponent state;
    state.current = StateId::BeingHit;

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<PlayerComponent>(entity, PlayerComponent{2});
    comp.add<StateComponent>(entity, state);

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++eventCount; });

    this->scene.systems().addSystem<AttackSystem>(this->bus, InputAction::Punch, TriggerId::Punched);
    this->scene.update(updateDelay);

    REQUIRE(eventCount == 0);
}

TEST_CASE_METHOD(AttackSystemFixture, "AttackSystem blocks attacks while frozen by hitstop",
    "[integration][attack_system]"
) {
    const auto& updateDelay = AttackSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Kick] = InputComponent::State{true, 0.f};

    HitstopComponent hitstop{ .remaining = 1.f, .frozen = true };

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<PlayerComponent>(entity, PlayerComponent{3});
    comp.add<HitstopComponent>(entity, hitstop);

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++eventCount; });

    this->scene.systems().addSystem<AttackSystem>(this->bus, InputAction::Kick, TriggerId::Kicked);
    this->scene.update(updateDelay);

    REQUIRE(eventCount == 0);
}
