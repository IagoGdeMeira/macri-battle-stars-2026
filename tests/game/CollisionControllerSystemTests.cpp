#include "game/include/CollisionControllerSystem/CollisionControllerSystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/HitboxControllerComponent.h"
#include "domain/components/HitboxControllerMapComponent.h"
#include "domain/components/HurtboxControllerComponent.h"
#include "domain/components/HurtboxControllerMapComponent.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/components/PushboxControllerMapComponent.h"
#include "domain/components/StateComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include "game/collision_controllers/HitboxCollisionController.h"
#include "game/collision_controllers/HurtboxCollisionController.h"
#include "game/collision_controllers/PushboxCollisionController.h"
#include "game/events/StateChangedEvent.h"

#include <catch2/catch_test_macros.hpp>

class CollisionControllerSystemFixture
{
public:
    CollisionControllerSystemFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<ActiveComponent>();
        comp.registerComponent<HitboxControllerMapComponent>();
        comp.registerComponent<HitboxControllerComponent>();
        comp.registerComponent<HurtboxControllerMapComponent>();
        comp.registerComponent<HurtboxControllerComponent>();
        comp.registerComponent<OrientationComponent>();
        comp.registerComponent<PushboxControllerMapComponent>();
        comp.registerComponent<PushboxControllerComponent>();
        comp.registerComponent<StateComponent>();

        this->system.addController(std::make_unique<HitboxCollisionController>());
        this->system.addController(std::make_unique<HurtboxCollisionController>());
        this->system.addController(std::make_unique<PushboxCollisionController>());
    }

    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    CollisionControllerSystem system{this->bus};
};

TEST_CASE_METHOD(CollisionControllerSystemFixture, "CollisionControllerSystem adds controllers on StateChangedEvent",
    "[unit][collision_controller_system]")
{
    auto& comp = this->world.components();
    Entity entity = this->world.entities().create();

    HitboxControllerMapComponent hbMap;
    hbMap.map[StateId::Punching] = HitboxControllerComponent{};
    comp.add<HitboxControllerMapComponent>(entity, std::move(hbMap));

    HurtboxControllerMapComponent huMap;
    huMap.map[StateId::Punching] = HurtboxControllerComponent{};
    comp.add<HurtboxControllerMapComponent>(entity, std::move(huMap));

    PushboxControllerMapComponent pbMap;
    pbMap.map[StateId::Punching] = PushboxControllerComponent{};
    comp.add<PushboxControllerMapComponent>(entity, std::move(pbMap));

    this->bus.emit<StateChangedEvent>(entity, StateId::Idle, StateId::Punching);

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};
    this->system.update(ctx);

    REQUIRE(comp.has<HitboxControllerComponent>(entity));
    REQUIRE(comp.has<HurtboxControllerComponent>(entity));
    REQUIRE(comp.has<PushboxControllerComponent>(entity));
}

TEST_CASE_METHOD(CollisionControllerSystemFixture, "CollisionControllerSystem removes controllers when state not in map",
    "[unit][collision_controller_system]")
{
    auto& comp = this->world.components();
    Entity entity = this->world.entities().create();

    HitboxControllerMapComponent hbMap;
    comp.add<HitboxControllerMapComponent>(entity, std::move(hbMap));
    comp.add<HitboxControllerComponent>(entity, HitboxControllerComponent{});

    HurtboxControllerMapComponent huMap;
    comp.add<HurtboxControllerMapComponent>(entity, std::move(huMap));
    comp.add<HurtboxControllerComponent>(entity, HurtboxControllerComponent{});

    PushboxControllerMapComponent pbMap;
    comp.add<PushboxControllerMapComponent>(entity, std::move(pbMap));
    comp.add<PushboxControllerComponent>(entity, PushboxControllerComponent{});

    this->bus.emit<StateChangedEvent>(entity, StateId::Idle, StateId::Blocking);

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};
    this->system.update(ctx);

    REQUIRE_FALSE(comp.has<HitboxControllerComponent>(entity));
    REQUIRE_FALSE(comp.has<HurtboxControllerComponent>(entity));
    REQUIRE_FALSE(comp.has<PushboxControllerComponent>(entity));
}

TEST_CASE_METHOD(CollisionControllerSystemFixture, "CollisionControllerSystem proactively initializes controllers on Idle",
    "[unit][collision_controller_system]")
{
    auto& comp = this->world.components();
    Entity entity = this->world.entities().create();

    comp.add<StateComponent>(entity, StateComponent{StateId::Idle});
    HitboxControllerMapComponent hbMap;
    hbMap.map[StateId::Idle] = HitboxControllerComponent{};
    comp.add<HitboxControllerMapComponent>(entity, std::move(hbMap));

    HurtboxControllerMapComponent huMap;
    huMap.map[StateId::Idle] = HurtboxControllerComponent{};
    comp.add<HurtboxControllerMapComponent>(entity, std::move(huMap));

    PushboxControllerMapComponent pbMap;
    pbMap.map[StateId::Idle] = PushboxControllerComponent{};
    comp.add<PushboxControllerMapComponent>(entity, std::move(pbMap));

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};
    this->system.update(ctx);

    REQUIRE(comp.has<HitboxControllerComponent>(entity));
    REQUIRE(comp.has<HurtboxControllerComponent>(entity));
    REQUIRE(comp.has<PushboxControllerComponent>(entity));
}

TEST_CASE_METHOD(CollisionControllerSystemFixture, "CollisionControllerSystem handles multiple entities with different maps",
    "[unit][collision_controller_system]")
{
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    Entity e1 = entities.create();
    comp.add<StateComponent>(e1, StateComponent{StateId::Idle});
    HitboxControllerMapComponent map1;
    map1.map[StateId::Idle] = HitboxControllerComponent{};
    comp.add<HitboxControllerMapComponent>(e1, std::move(map1));

    Entity e2 = entities.create();
    comp.add<StateComponent>(e2, StateComponent{StateId::Idle});

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};
    this->system.update(ctx);

    REQUIRE(comp.has<HitboxControllerComponent>(e1));
    REQUIRE_FALSE(comp.has<HitboxControllerComponent>(e2));
}

TEST_CASE_METHOD(CollisionControllerSystemFixture, "CollisionControllerSystem does not duplicate proactive init if controller already exists",
    "[unit][collision_controller_system]")
{
    auto& comp = this->world.components();
    Entity entity = this->world.entities().create();

    comp.add<StateComponent>(entity, StateComponent{StateId::Idle});
    HitboxControllerMapComponent map;
    map.map[StateId::Idle] = HitboxControllerComponent{};
    comp.add<HitboxControllerMapComponent>(entity, std::move(map));
    comp.add<HitboxControllerComponent>(entity, HitboxControllerComponent{});

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};
    REQUIRE_NOTHROW(this->system.update(ctx));
}
