#include "../../src/game/include/FallTriggerSystem/FallTriggerSystem.h"

#include "../../src/domain/components/GroundedComponent.h"
#include "../../src/domain/components/PlayerComponent.h"
#include "../../src/game/events/TriggerEvent.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/Scene/Scene.h"

#include <catch2/catch_test_macros.hpp>
#include <vector>

class FallTriggerSystemFixture
{
public:
    EventBus bus;
    Scene scene{this->bus};

    static constexpr float UPDATE_DELAY = 0.016f;

    FallTriggerSystemFixture()
    {
        auto& comp = this->scene.world().components();
        comp.registerComponent<GroundedComponent>();
        comp.registerComponent<PlayerComponent>();
    }
};

TEST_CASE_METHOD(FallTriggerSystemFixture, "FallTriggerSystem emits a fall trigger when grounded state changes to airborne",
    "[integration][fall_trigger_system]"
) {
    const auto& updateDelay = FallTriggerSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    GroundedComponent grounded;
    grounded.onGround = true;

    auto& comp = this->scene.world().components();
    comp.add<GroundedComponent>(entity, grounded);
    comp.add<PlayerComponent>(entity, PlayerComponent{1});

    std::vector<TriggerEvent> events;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent& event) { events.push_back(event); });

    this->scene.systems().addSystem<FallTriggerSystem>(this->bus);

    auto& storedGrounded = comp.get<GroundedComponent>(entity);
    this->scene.update(updateDelay);

    storedGrounded.onGround = false;
    this->scene.update(updateDelay);

    REQUIRE(events.size() == 1);
    REQUIRE(events[0].entity == entity);
    REQUIRE(events[0].trigger == TriggerId::Fall);
}

TEST_CASE_METHOD(FallTriggerSystemFixture, "FallTriggerSystem does not emit fall on the initial grounded frame",
    "[integration][fall_trigger_system]"
) {
    const auto& updateDelay = FallTriggerSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    GroundedComponent grounded;
    grounded.onGround = false;

    auto& comp = this->scene.world().components();
    comp.add<GroundedComponent>(entity, grounded);
    comp.add<PlayerComponent>(entity, PlayerComponent{2});

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++eventCount; });

    this->scene.systems().addSystem<FallTriggerSystem>(this->bus);
    this->scene.update(updateDelay);

    REQUIRE(eventCount == 0);
}

TEST_CASE_METHOD(FallTriggerSystemFixture, "FallTriggerSystem does not repeat fall triggers while the entity remains airborne",
    "[integration][fall_trigger_system]"
) {
    const auto& updateDelay = FallTriggerSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    GroundedComponent grounded;
    grounded.onGround = true;

    auto& comp = this->scene.world().components();
    comp.add<GroundedComponent>(entity, grounded);
    comp.add<PlayerComponent>(entity, PlayerComponent{3});

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++eventCount; });

    this->scene.systems().addSystem<FallTriggerSystem>(this->bus);

    auto& storedGrounded = comp.get<GroundedComponent>(entity);
    this->scene.update(updateDelay);
    storedGrounded.onGround = false;
    this->scene.update(updateDelay);
    this->scene.update(updateDelay);

    REQUIRE(eventCount == 1);
}
