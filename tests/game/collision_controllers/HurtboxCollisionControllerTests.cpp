#include "game/collision_controllers/HurtboxCollisionController.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/HurtboxComponent.h"
#include "domain/components/HurtboxControllerComponent.h"
#include "domain/components/HurtboxControllerMapComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/TransformComponent.h"
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

TEST_CASE_METHOD(HurtboxCollisionControllerFixture, "onOrientationChanged inverts LocalTransform.x for active frame hurtboxes",
    "[unit][hurtbox_collision_controller]")
{
    auto& comp = this->world.components();
    comp.registerComponent<TransformComponent>();
    comp.registerComponent<LocalTransform>();
    comp.registerComponent<ParentComponent>();
    comp.registerComponent<HurtboxComponent>();
    comp.registerComponent<OrientationComponent>();

    Entity parent = this->world.entities().create();
    comp.add<TransformComponent>(parent, TransformComponent{100.f, 200.f});
    comp.add<OrientationComponent>(parent, OrientationComponent{Orientation::Left});

    Entity child = this->world.entities().create();
    comp.add<LocalTransform>(child, LocalTransform{{30.f, 10.f}});
    comp.add<ParentComponent>(child, ParentComponent{parent});
    comp.add<TransformComponent>(child, TransformComponent{});
    comp.add<HurtboxComponent>(child, HurtboxComponent{});

    HurtboxControllerComponent ctrl;
    ctrl.frames = {{ {child}, 0.1f }};
    ctrl.initialized = true;
    comp.add<HurtboxControllerComponent>(parent, std::move(ctrl));

    ICollisionController::ControllerParams params{parent, this->world};
    this->controller.onOrientationChanged(params);

    const auto& local = comp.get<LocalTransform>(child);
    REQUIRE(local.position.x == -30.f);
}

TEST_CASE_METHOD(HurtboxCollisionControllerFixture, "onOrientationChanged does nothing without controller component",
    "[unit][hurtbox_collision_controller]")
{
    auto& comp = this->world.components();
    comp.registerComponent<OrientationComponent>();
    Entity parent = this->world.entities().create();
    comp.add<OrientationComponent>(parent, OrientationComponent{Orientation::Left});

    ICollisionController::ControllerParams params{parent, this->world};
    REQUIRE_NOTHROW(this->controller.onOrientationChanged(params));
}

TEST_CASE_METHOD(HurtboxCollisionControllerFixture, "onOrientationChanged does nothing if controller not initialized",
    "[unit][hurtbox_collision_controller]")
{
    auto& comp = this->world.components();
    comp.registerComponent<OrientationComponent>();
    Entity parent = this->world.entities().create();
    comp.add<OrientationComponent>(parent, OrientationComponent{Orientation::Left});
    comp.add<HurtboxControllerComponent>(parent, HurtboxControllerComponent{});

    ICollisionController::ControllerParams params{parent, this->world};
    REQUIRE_NOTHROW(this->controller.onOrientationChanged(params));
}

TEST_CASE_METHOD(HurtboxCollisionControllerFixture, "onOrientationChanged only affects current frame entities",
    "[unit][hurtbox_collision_controller]")
{
    auto& comp = this->world.components();
    comp.registerComponent<TransformComponent>();
    comp.registerComponent<LocalTransform>();
    comp.registerComponent<ParentComponent>();
    comp.registerComponent<HurtboxComponent>();
    comp.registerComponent<OrientationComponent>();

    Entity parent = this->world.entities().create();
    comp.add<TransformComponent>(parent, TransformComponent{});
    comp.add<OrientationComponent>(parent, OrientationComponent{Orientation::Right});

    Entity child1 = this->world.entities().create();
    comp.add<LocalTransform>(child1, LocalTransform{{10.f, 0.f}});
    comp.add<ParentComponent>(child1, ParentComponent{parent});
    comp.add<TransformComponent>(child1, TransformComponent{});
    comp.add<HurtboxComponent>(child1, HurtboxComponent{});

    Entity child2 = this->world.entities().create();
    comp.add<LocalTransform>(child2, LocalTransform{{20.f, 0.f}});
    comp.add<ParentComponent>(child2, ParentComponent{parent});
    comp.add<TransformComponent>(child2, TransformComponent{});
    comp.add<HurtboxComponent>(child2, HurtboxComponent{});

    HurtboxControllerComponent ctrl;
    ctrl.frames = {{{child1}, 0.1f}, {{child2}, 0.1f}};
    ctrl.currentFrame = 0;
    ctrl.initialized = true;
    comp.add<HurtboxControllerComponent>(parent, std::move(ctrl));

    ICollisionController::ControllerParams params{parent, this->world};
    this->controller.onOrientationChanged(params);

    REQUIRE(comp.get<LocalTransform>(child1).position.x == 10.f);
    REQUIRE(comp.get<LocalTransform>(child2).position.x == 20.f);
}
