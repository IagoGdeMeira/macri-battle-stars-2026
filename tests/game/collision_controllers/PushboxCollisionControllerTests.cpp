#include "game/collision_controllers/PushboxCollisionController.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/components/PushboxControllerMapComponent.h"
#include "domain/include/World/World.h"

#include <catch2/catch_test_macros.hpp>

class PushboxCollisionControllerFixture
{
public:
    PushboxCollisionControllerFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<PushboxControllerMapComponent>();
        comp.registerComponent<PushboxControllerComponent>();
        comp.registerComponent<ActiveComponent>();
    }

    World world;
    PushboxCollisionController controller;

    Entity createEntityWithMap(StateId state, const PushboxControllerComponent& ctrl = {})
    {
        Entity e = this->world.entities().create();
        PushboxControllerMapComponent map;
        map.map[state] = ctrl;
        this->world.components().add<PushboxControllerMapComponent>(e, std::move(map));
        return e;
    }

    Entity createEntityWithController(const PushboxControllerComponent& ctrl = {})
    {
        Entity e = this->world.entities().create();
        this->world.components().add<PushboxControllerComponent>(e, ctrl);
        return e;
    }
};

TEST_CASE_METHOD(PushboxCollisionControllerFixture, "hasMapComponent returns true when entity has PushboxControllerMapComponent",
    "[unit][pushbox_collision_controller]")
{
    Entity entity = this->createEntityWithMap(StateId::Idle);
    ICollisionController::ControllerParams params{entity, this->world};
    REQUIRE(this->controller.hasMapComponent(params));
}

TEST_CASE_METHOD(PushboxCollisionControllerFixture, "hasController returns true when entity has PushboxControllerComponent",
    "[unit][pushbox_collision_controller]")
{
    Entity entity = this->createEntityWithController();
    ICollisionController::ControllerParams params{entity, this->world};
    REQUIRE(this->controller.hasController(params));
}

TEST_CASE_METHOD(PushboxCollisionControllerFixture, "apply adds controller when state exists in map",
    "[unit][pushbox_collision_controller]")
{
    PushboxControllerComponent expectedCtrl;
    expectedCtrl.frames = {{ {}, 0.1f }};
    Entity entity = this->createEntityWithMap(StateId::Punching, expectedCtrl);

    ICollisionController::ControllerParams params{entity, this->world};
    this->controller.apply(params, StateId::Punching);

    REQUIRE(this->world.components().has<PushboxControllerComponent>(entity));
}

TEST_CASE_METHOD(PushboxCollisionControllerFixture, "apply removes controller when state not in map",
    "[unit][pushbox_collision_controller]")
{
    auto& comp = this->world.components();
    Entity entity = this->createEntityWithMap(StateId::Idle);
    comp.add<PushboxControllerComponent>(entity, PushboxControllerComponent{});

    ICollisionController::ControllerParams params{entity, this->world};
    this->controller.apply(params, StateId::Blocking);

    REQUIRE_FALSE(comp.has<PushboxControllerComponent>(entity));
}

TEST_CASE_METHOD(PushboxCollisionControllerFixture, "remove deactivates current frame and removes controller",
    "[unit][pushbox_collision_controller]")
{
    auto& comp = this->world.components();
    Entity child = this->world.entities().create();
    comp.add<ActiveComponent>(child, ActiveComponent{true});

    PushboxControllerComponent ctrl;
    ctrl.frames = {{ {child}, 0.1f }};
    ctrl.initialized = true;
    Entity entity = this->createEntityWithController(ctrl);

    ICollisionController::ControllerParams params{entity, this->world};
    this->controller.remove(params);

    REQUIRE_FALSE(comp.has<PushboxControllerComponent>(entity));
    REQUIRE(comp.get<ActiveComponent>(child).active == false);
}
