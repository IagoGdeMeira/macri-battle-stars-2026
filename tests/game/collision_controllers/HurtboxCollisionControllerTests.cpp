#include "game/collision_controllers/HurtboxCollisionController.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/HurtboxControllerComponent.h"
#include "domain/components/HurtboxControllerMapComponent.h"
#include "domain/include/World/World.h"

#include <catch2/catch_test_macros.hpp>

class HurtboxCollisionControllerFixture
{
public:
    HurtboxCollisionControllerFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<HurtboxControllerMapComponent>();
        comp.registerComponent<HurtboxControllerComponent>();
        comp.registerComponent<ActiveComponent>();
    }

    World world;
    HurtboxCollisionController controller;

    Entity createEntityWithMap(StateId state, const HurtboxControllerComponent& ctrl = {})
    {
        Entity e = this->world.entities().create();
        HurtboxControllerMapComponent map;
        map.map[state] = ctrl;
        this->world.components().add<HurtboxControllerMapComponent>(e, std::move(map));
        return e;
    }

    Entity createEntityWithController(const HurtboxControllerComponent& ctrl = {})
    {
        Entity e = this->world.entities().create();
        this->world.components().add<HurtboxControllerComponent>(e, ctrl);
        return e;
    }
};

TEST_CASE_METHOD(HurtboxCollisionControllerFixture, "hasMapComponent returns true when entity has HurtboxControllerMapComponent",
    "[unit][hurtbox_collision_controller]")
{
    Entity entity = this->createEntityWithMap(StateId::Idle);
    ICollisionController::ControllerParams params{entity, this->world};
    REQUIRE(this->controller.hasMapComponent(params));
}

TEST_CASE_METHOD(HurtboxCollisionControllerFixture, "hasController returns true when entity has HurtboxControllerComponent",
    "[unit][hurtbox_collision_controller]")
{
    Entity entity = this->createEntityWithController();
    ICollisionController::ControllerParams params{entity, this->world};
    REQUIRE(this->controller.hasController(params));
}

TEST_CASE_METHOD(HurtboxCollisionControllerFixture, "apply adds controller when state exists in map",
    "[unit][hurtbox_collision_controller]")
{
    HurtboxControllerComponent expectedCtrl;
    expectedCtrl.frames = {{ {}, 0.1f }};
    Entity entity = this->createEntityWithMap(StateId::Punching, expectedCtrl);

    ICollisionController::ControllerParams params{entity, this->world};
    this->controller.apply(params, StateId::Punching);

    REQUIRE(this->world.components().has<HurtboxControllerComponent>(entity));
}

TEST_CASE_METHOD(HurtboxCollisionControllerFixture, "apply removes controller when state not in map",
    "[unit][hurtbox_collision_controller]")
{
    Entity entity = this->createEntityWithMap(StateId::Idle);
    auto& comp = this->world.components();
    comp.add<HurtboxControllerComponent>(entity, HurtboxControllerComponent{});

    ICollisionController::ControllerParams params{entity, this->world};
    this->controller.apply(params, StateId::Blocking);

    REQUIRE_FALSE(comp.has<HurtboxControllerComponent>(entity));
}

TEST_CASE_METHOD(HurtboxCollisionControllerFixture, "remove deactivates current frame and removes controller",
    "[unit][hurtbox_collision_controller]")
{
    auto& comp = this->world.components();
    Entity child = this->world.entities().create();
    comp.add<ActiveComponent>(child, ActiveComponent{true});

    HurtboxControllerComponent ctrl;
    ctrl.frames = {{ {child}, 0.1f }};
    ctrl.initialized = true;
    Entity entity = this->createEntityWithController(ctrl);

    ICollisionController::ControllerParams params{entity, this->world};
    this->controller.remove(params);

    REQUIRE_FALSE(comp.has<HurtboxControllerComponent>(entity));
    REQUIRE(comp.get<ActiveComponent>(child).active == false);
}
