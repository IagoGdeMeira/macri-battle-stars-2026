#include "game/include/CrouchSystem/CrouchSystem.h"

#include "domain/components/GroundedComponent.h"
#include "domain/components/HitstopComponent.h"
#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/value_objects/InputAction/InputAction.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/Scene/Scene.h"

#include "game/events/TriggerEvent.h"

#include <catch2/catch_test_macros.hpp>
#include <vector>

class CrouchSystemFixture
{
public:
    EventBus bus;
    Scene scene{this->bus};

    static constexpr float UPDATE_DELAY = 0.016f;

    CrouchSystemFixture()
    {
        auto& comp = this->scene.world().components();
        comp.registerComponent<GroundedComponent>();
        comp.registerComponent<HitstopComponent>();
        comp.registerComponent<InputComponent>();
        comp.registerComponent<PlayerComponent>();
    }
};

TEST_CASE_METHOD(CrouchSystemFixture, "CrouchSystem emits crouch and crouch release triggers while grounded",
    "[integration][crouch_system]"
) {
    const auto& updateDelay = CrouchSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    GroundedComponent grounded;
    grounded.onGround = true;

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<GroundedComponent>(entity, grounded);
    comp.add<PlayerComponent>(entity, PlayerComponent{1});

    std::vector<TriggerEvent> events;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent& event) { events.push_back(event); });

    this->scene.systems().addSystem<CrouchSystem>(this->bus);

    auto& storedInput = comp.get<InputComponent>(entity);
    storedInput.actions[InputAction::Crouch] = InputComponent::State{true, 0.f};
    this->scene.update(updateDelay);

    storedInput.actions[InputAction::Crouch] = InputComponent::State{false, 0.f};
    this->scene.update(updateDelay);

    REQUIRE(events.size() == 2);
    REQUIRE(events[0].entity == entity);
    REQUIRE(events[0].trigger == TriggerId::Crouched);
    REQUIRE(events[1].entity == entity);
    REQUIRE(events[1].trigger == TriggerId::CrouchReleased);
}

TEST_CASE_METHOD(CrouchSystemFixture, "CrouchSystem ignores crouch press when entity is not grounded",
    "[integration][crouch_system]"
) {
    const auto& updateDelay = CrouchSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Crouch] = InputComponent::State{true, 0.f};

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<PlayerComponent>(entity, PlayerComponent{2});

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++eventCount; });

    this->scene.systems().addSystem<CrouchSystem>(this->bus);
    this->scene.update(updateDelay);

    REQUIRE(eventCount == 0);
}

TEST_CASE_METHOD(CrouchSystemFixture, "CrouchSystem does not repeat crouch triggers while the input is held",
    "[integration][crouch_system]"
) {
    const auto& updateDelay = CrouchSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    InputComponent input;
    input.actions[InputAction::Crouch] = InputComponent::State{true, 0.f};

    GroundedComponent grounded;
    grounded.onGround = true;

    auto& comp = this->scene.world().components();
    comp.add<InputComponent>(entity, input);
    comp.add<GroundedComponent>(entity, grounded);
    comp.add<PlayerComponent>(entity, PlayerComponent{3});

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++eventCount; });

    this->scene.systems().addSystem<CrouchSystem>(this->bus);
    this->scene.update(updateDelay);
    this->scene.update(updateDelay);

    REQUIRE(eventCount == 1);
}
