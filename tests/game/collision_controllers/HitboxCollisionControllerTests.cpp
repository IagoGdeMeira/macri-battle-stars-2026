#include "game/collision_controllers/HitboxCollisionController.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/HitboxControllerComponent.h"
#include "domain/components/HitboxControllerMapComponent.h"
#include "domain/include/World/World.h"

#include <catch2/catch_test_macros.hpp>

class HitboxCollisionControllerFixture
{
public:
    HitboxCollisionControllerFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<HitboxControllerMapComponent>();
        comp.registerComponent<HitboxControllerComponent>();
        comp.registerComponent<ActiveComponent>();
    }

    World world;
    HitboxCollisionController controller;

    Entity createEntityWithMap(StateId state, const HitboxControllerComponent& ctrl = {})
    {
        Entity e = this->world.entities().create();
        HitboxControllerMapComponent map;
        map.map[state] = ctrl;
        this->world.components().add<HitboxControllerMapComponent>(e, std::move(map));
        return e;
    }

    Entity createEntityWithController(const HitboxControllerComponent& ctrl = {})
    {
        Entity e = this->world.entities().create();
        this->world.components().add<HitboxControllerComponent>(e, ctrl);
        return e;
    }
};

TEST_CASE_METHOD(HitboxCollisionControllerFixture, "hasMapComponent returns true when entity has HitboxControllerMapComponent",
    "[unit][hitbox_collision_controller]")
{
    Entity entity = this->createEntityWithMap(StateId::Idle);
    ICollisionController::ControllerParams params{entity, this->world};
    REQUIRE(this->controller.hasMapComponent(params));
}

TEST_CASE_METHOD(HitboxCollisionControllerFixture, "hasMapComponent returns false without map component",
    "[unit][hitbox_collision_controller]")
{
    Entity entity = this->world.entities().create();
    ICollisionController::ControllerParams params{entity, this->world};
    REQUIRE_FALSE(this->controller.hasMapComponent(params));
}

TEST_CASE_METHOD(HitboxCollisionControllerFixture, "hasController returns true when entity has HitboxControllerComponent",
    "[unit][hitbox_collision_controller]")
{
    Entity entity = this->createEntityWithController();
    ICollisionController::ControllerParams params{entity, this->world};
    REQUIRE(this->controller.hasController(params));
}

TEST_CASE_METHOD(HitboxCollisionControllerFixture, "hasController returns false without controller component",
    "[unit][hitbox_collision_controller]")
{
    Entity entity = this->world.entities().create();
    ICollisionController::ControllerParams params{entity, this->world};
    REQUIRE_FALSE(this->controller.hasController(params));
}

TEST_CASE_METHOD(HitboxCollisionControllerFixture, "apply adds controller when state exists in map",
    "[unit][hitbox_collision_controller]")
{
    HitboxControllerComponent expectedCtrl;
    expectedCtrl.frames = {{ {}, 0.1f }};
    expectedCtrl.loop = false;
    Entity entity = this->createEntityWithMap(StateId::Punching, expectedCtrl);

    ICollisionController::ControllerParams params{entity, this->world};
    this->controller.apply(params, StateId::Punching);

    auto& comp = this->world.components();
    REQUIRE(comp.has<HitboxControllerComponent>(entity));
    const auto& ctrl = comp.get<HitboxControllerComponent>(entity);
    REQUIRE(ctrl.frames.size() == 1);
    REQUIRE(ctrl.loop == false);
    REQUIRE(ctrl.initialized == true);
}

TEST_CASE_METHOD(HitboxCollisionControllerFixture, "apply removes controller when state not in map",
    "[unit][hitbox_collision_controller]")
{
    auto& comp = this->world.components();
    Entity entity = this->createEntityWithMap(StateId::Idle);
    comp.add<HitboxControllerComponent>(entity, HitboxControllerComponent{});

    ICollisionController::ControllerParams params{entity, this->world};
    this->controller.apply(params, StateId::Blocking);

    REQUIRE_FALSE(comp.has<HitboxControllerComponent>(entity));
}

TEST_CASE_METHOD(HitboxCollisionControllerFixture, "apply replaces existing controller on state change",
    "[unit][hitbox_collision_controller]")
{
    Entity entity = this->world.entities().create();
    auto& comp = this->world.components();
    HitboxControllerMapComponent map;
    HitboxControllerComponent idleCtrl;
    idleCtrl.frames = {{ {}, 0.1f }};
    idleCtrl.initialized = true;
    map.map[StateId::Idle] = idleCtrl;
    HitboxControllerComponent punchCtrl;
    punchCtrl.frames = {{ {}, 0.05f }, {{}, 0.05f}};
    map.map[StateId::Punching] = punchCtrl;
    comp.add<HitboxControllerMapComponent>(entity, std::move(map));
    comp.add<HitboxControllerComponent>(entity, idleCtrl);

    ICollisionController::ControllerParams params{entity, this->world};
    this->controller.apply(params, StateId::Punching);

    REQUIRE(comp.has<HitboxControllerComponent>(entity));
    const auto& ctrl = comp.get<HitboxControllerComponent>(entity);
    REQUIRE(ctrl.frames.size() == 2);
    REQUIRE(ctrl.currentFrame == 0);
    REQUIRE(ctrl.elapsedTime == 0.f);
    REQUIRE(ctrl.initialized == true);
}

TEST_CASE_METHOD(HitboxCollisionControllerFixture, "remove deactivates current frame and removes controller",
    "[unit][hitbox_collision_controller]")
{
    auto& comp = this->world.components();
    Entity child = this->world.entities().create();
    comp.add<ActiveComponent>(child, ActiveComponent{true});

    HitboxControllerComponent ctrl;
    ctrl.frames = {{ {child}, 0.1f }};
    ctrl.initialized = true;
    Entity entity = this->createEntityWithController(ctrl);

    ICollisionController::ControllerParams params{entity, this->world};
    this->controller.remove(params);

    REQUIRE_FALSE(comp.has<HitboxControllerComponent>(entity));
    REQUIRE(comp.get<ActiveComponent>(child).active == false);
}
