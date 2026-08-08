#include "game/include/FallTriggerSystem/FallTriggerSystem.h"

#include "domain/components/GroundedComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/components/VelocityComponent.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/Scene/Scene.h"

#include "game/events/TriggerEvent.h"

#include <catch2/catch_approx.hpp>
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
        comp.registerComponent<VelocityComponent>();
    }
};

TEST_CASE_METHOD(FallTriggerSystemFixture, "FallTriggerSystem emits fall trigger when velocity becomes positive while airborne",
    "[integration][fall_trigger_system]"
) {
    const auto& updateDelay = FallTriggerSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    GroundedComponent grounded;
    grounded.onGround = false;

    VelocityComponent velocity;
    velocity.velocity.y = -10.f;

    auto& comp = this->scene.world().components();
    comp.add<GroundedComponent>(entity, grounded);
    comp.add<VelocityComponent>(entity, velocity);
    comp.add<PlayerComponent>(entity, PlayerComponent{1});

    std::vector<TriggerEvent> events;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent& event) { events.push_back(event); });

    this->scene.systems().addSystem<FallTriggerSystem>(this->bus);

    this->scene.update(updateDelay);
    REQUIRE(events.empty());

    comp.get<VelocityComponent>(entity).velocity.y = 5.f;
    this->scene.update(updateDelay);

    REQUIRE(events.size() == 1);
    REQUIRE(events[0].entity == entity);
    REQUIRE(events[0].trigger == TriggerId::Fall);
}

TEST_CASE_METHOD(FallTriggerSystemFixture, "FallTriggerSystem does not emit fall when grounded",
    "[integration][fall_trigger_system]"
) {
    const auto& updateDelay = FallTriggerSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    GroundedComponent grounded;
    grounded.onGround = true;

    VelocityComponent velocity;
    velocity.velocity.y = 5.f;

    auto& comp = this->scene.world().components();
    comp.add<GroundedComponent>(entity, grounded);
    comp.add<VelocityComponent>(entity, velocity);
    comp.add<PlayerComponent>(entity, PlayerComponent{2});

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++eventCount; });

    this->scene.systems().addSystem<FallTriggerSystem>(this->bus);
    this->scene.update(updateDelay);

    REQUIRE(eventCount == 0);
}

TEST_CASE_METHOD(FallTriggerSystemFixture, "FallTriggerSystem does not repeat fall triggers while velocity remains positive",
    "[integration][fall_trigger_system]"
) {
    const auto& updateDelay = FallTriggerSystemFixture::UPDATE_DELAY;
    const auto entity = this->scene.world().entities().create();

    GroundedComponent grounded;
    grounded.onGround = false;

    VelocityComponent velocity;
    velocity.velocity.y = -2.f;

    auto& comp = this->scene.world().components();
    comp.add<GroundedComponent>(entity, grounded);
    comp.add<VelocityComponent>(entity, velocity);
    comp.add<PlayerComponent>(entity, PlayerComponent{3});

    int eventCount = 0;
    this->bus.subscribe<TriggerEvent>([&](const TriggerEvent&) { ++eventCount; });

    this->scene.systems().addSystem<FallTriggerSystem>(this->bus);

    this->scene.update(updateDelay);

    comp.get<VelocityComponent>(entity).velocity.y = 1.f;
    this->scene.update(updateDelay);
    REQUIRE(eventCount == 1);

    comp.get<VelocityComponent>(entity).velocity.y = 10.f;
    this->scene.update(updateDelay);
    REQUIRE(eventCount == 1);
}
