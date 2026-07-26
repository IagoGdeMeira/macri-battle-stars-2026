#include "game/include/CollisionControllerSystem/CollisionControllerSystem.h"

#include "domain/components/HitboxControllerMapComponent.h"
#include "domain/components/HitboxControllerComponent.h"
#include "domain/components/HurtboxControllerMapComponent.h"
#include "domain/components/HurtboxControllerComponent.h"
#include "domain/components/PushboxControllerMapComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include "game/events/StateChangedEvent.h"

#include <catch2/catch_test_macros.hpp>

class CollisionControllerSystemFixture
{
public:
    CollisionControllerSystemFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<HitboxControllerMapComponent>();
        comp.registerComponent<HitboxControllerComponent>();
        comp.registerComponent<HurtboxControllerMapComponent>();
        comp.registerComponent<HurtboxControllerComponent>();
        comp.registerComponent<PushboxControllerMapComponent>();
        comp.registerComponent<PushboxControllerComponent>();
    }

    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    CollisionControllerSystem system{this->bus};
};

TEST_CASE_METHOD(CollisionControllerSystemFixture, "CollisionControllerSystem adds HitboxController when state has map entry",
    "[unit][collision_controller_system]"
) {
    auto& comp = this->world.components();

    Entity entity = this->world.entities().create();

    HitboxControllerMapComponent map;
    map.map[StateId::Punching] = HitboxControllerComponent{};
    comp.add<HitboxControllerMapComponent>(entity, std::move(map));

    this->bus.emit<StateChangedEvent>(entity, StateId::Idle, StateId::Punching);

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};
    this->system.update(ctx);

    REQUIRE(comp.has<HitboxControllerComponent>(entity));
}

TEST_CASE_METHOD(CollisionControllerSystemFixture, "CollisionControllerSystem removes HitboxController when state has no map entry",
    "[unit][collision_controller_system]"
) {
    auto& comp = this->world.components();

    Entity entity = this->world.entities().create();

    HitboxControllerMapComponent map;
    comp.add<HitboxControllerMapComponent>(entity, std::move(map));

    this->bus.emit<StateChangedEvent>(entity, StateId::Idle, StateId::Blocking);

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};
    this->system.update(ctx);

    REQUIRE_FALSE(comp.has<HitboxControllerComponent>(entity));
}

TEST_CASE_METHOD(CollisionControllerSystemFixture, "CollisionControllerSystem adds HurtboxController when state has map entry",
    "[unit][collision_controller_system]"
) {
    auto& comp = this->world.components();

    Entity entity = this->world.entities().create();

    HurtboxControllerMapComponent map;
    map.map[StateId::Punching] = HurtboxControllerComponent{};
    comp.add<HurtboxControllerMapComponent>(entity, std::move(map));

    this->bus.emit<StateChangedEvent>(entity, StateId::Idle, StateId::Punching);

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};
    this->system.update(ctx);

    REQUIRE(comp.has<HurtboxControllerComponent>(entity));
}

TEST_CASE_METHOD(CollisionControllerSystemFixture, "CollisionControllerSystem removes HurtboxController when state has no map entry",
    "[unit][collision_controller_system]"
) {
    auto& comp = this->world.components();

    Entity entity = this->world.entities().create();

    HurtboxControllerMapComponent map;
    comp.add<HurtboxControllerMapComponent>(entity, std::move(map));

    this->bus.emit<StateChangedEvent>(entity, StateId::Idle, StateId::Blocking);

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};
    this->system.update(ctx);

    REQUIRE_FALSE(comp.has<HurtboxControllerComponent>(entity));
}

TEST_CASE_METHOD(CollisionControllerSystemFixture, "CollisionControllerSystem adds PushboxController when state has map entry",
    "[unit][collision_controller_system]"
) {
    auto& comp = this->world.components();

    Entity entity = this->world.entities().create();

    PushboxControllerMapComponent map;
    map.map[StateId::Punching] = PushboxControllerComponent{};
    comp.add<PushboxControllerMapComponent>(entity, std::move(map));

    this->bus.emit<StateChangedEvent>(entity, StateId::Idle, StateId::Punching);

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};
    this->system.update(ctx);

    REQUIRE(comp.has<PushboxControllerComponent>(entity));
}

TEST_CASE_METHOD(CollisionControllerSystemFixture, "CollisionControllerSystem removes PushboxController when state has no map entry",
    "[unit][collision_controller_system]"
) {
    auto& comp = this->world.components();

    Entity entity = this->world.entities().create();

    PushboxControllerMapComponent map;
    comp.add<PushboxControllerMapComponent>(entity, std::move(map));

    this->bus.emit<StateChangedEvent>(entity, StateId::Idle, StateId::Blocking);

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};
    this->system.update(ctx);

    REQUIRE_FALSE(comp.has<PushboxControllerComponent>(entity));
}

TEST_CASE_METHOD(CollisionControllerSystemFixture, "CollisionControllerSystem handles multiple entities independently",
    "[unit][collision_controller_system]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    Entity entity1 = entities.create();
    Entity entity2 = entities.create();

    HitboxControllerMapComponent map1;
    map1.map[StateId::Punching] = HitboxControllerComponent{};
    comp.add<HitboxControllerMapComponent>(entity1, std::move(map1));

    HitboxControllerMapComponent map2;
    comp.add<HitboxControllerMapComponent>(entity2, std::move(map2));

    this->bus.emit<StateChangedEvent>(entity1, StateId::Idle, StateId::Punching);
    this->bus.emit<StateChangedEvent>(entity2, StateId::Idle, StateId::Punching);

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};
    this->system.update(ctx);

    REQUIRE(comp.has<HitboxControllerComponent>(entity1));
    REQUIRE_FALSE(comp.has<HitboxControllerComponent>(entity2));
}
