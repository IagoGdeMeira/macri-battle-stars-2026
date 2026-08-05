#include "game/collision_controllers/HitboxCollisionController.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/HitboxComponent.h"
#include "domain/components/HitboxControllerComponent.h"
#include "domain/components/HitboxControllerMapComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"

#include <catch2/catch_approx.hpp>
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

TEST_CASE_METHOD(HitboxCollisionControllerFixture, "onOrientationChanged inverts hitbox position for active frame",
    "[unit][hitbox_collision_controller]")
{
    auto& comp = this->world.components();
    comp.registerComponent<TransformComponent>();
    comp.registerComponent<LocalTransform>();
    comp.registerComponent<ParentComponent>();
    comp.registerComponent<HitboxComponent>();
    comp.registerComponent<OrientationComponent>();

    Entity parent = this->world.entities().create();
    comp.add<TransformComponent>(parent, TransformComponent{100.f, 200.f});
    comp.add<OrientationComponent>(parent, OrientationComponent{Orientation::Left});

    Entity child = this->world.entities().create();
    comp.add<LocalTransform>(child, LocalTransform{{30.f, 10.f}});
    comp.add<ParentComponent>(child, ParentComponent{parent});
    comp.add<TransformComponent>(child, TransformComponent{});
    comp.add<HitboxComponent>(child, HitboxComponent{});

    HitboxControllerComponent ctrl;
    ctrl.frames = {{ {child}, 0.1f }};
    ctrl.initialized = true;
    comp.add<HitboxControllerComponent>(parent, std::move(ctrl));

    ICollisionController::ControllerParams params{parent, this->world};
    this->controller.onOrientationChanged(params);

    const auto& childT = comp.get<TransformComponent>(child);
    REQUIRE(childT.position.x == Catch::Approx(70.f));
    REQUIRE(childT.position.y == Catch::Approx(210.f));
}

TEST_CASE_METHOD(HitboxCollisionControllerFixture, "onOrientationChanged does nothing without controller component",
    "[unit][hitbox_collision_controller]")
{
    auto& comp = this->world.components();
    comp.registerComponent<OrientationComponent>();
    Entity parent = this->world.entities().create();
    comp.add<OrientationComponent>(parent, OrientationComponent{Orientation::Left});

    ICollisionController::ControllerParams params{parent, this->world};
    REQUIRE_NOTHROW(this->controller.onOrientationChanged(params));
}

TEST_CASE_METHOD(HitboxCollisionControllerFixture, "onOrientationChanged does nothing if controller not initialized",
    "[unit][hitbox_collision_controller]")
{
    auto& comp = this->world.components();
    comp.registerComponent<OrientationComponent>();
    Entity parent = this->world.entities().create();
    comp.add<OrientationComponent>(parent, OrientationComponent{Orientation::Left});
    comp.add<HitboxControllerComponent>(parent, HitboxControllerComponent{});

    ICollisionController::ControllerParams params{parent, this->world};
    REQUIRE_NOTHROW(this->controller.onOrientationChanged(params));
}

TEST_CASE_METHOD(HitboxCollisionControllerFixture, "onOrientationChanged only affects current frame entities",
    "[unit][hitbox_collision_controller]")
{
    auto& comp = this->world.components();
    comp.registerComponent<TransformComponent>();
    comp.registerComponent<LocalTransform>();
    comp.registerComponent<ParentComponent>();
    comp.registerComponent<HitboxComponent>();
    comp.registerComponent<OrientationComponent>();

    Entity parent = this->world.entities().create();
    comp.add<TransformComponent>(parent, TransformComponent{});
    comp.add<OrientationComponent>(parent, OrientationComponent{Orientation::Right});

    Entity child1 = this->world.entities().create();
    comp.add<LocalTransform>(child1, LocalTransform{{10.f, 0.f}});
    comp.add<ParentComponent>(child1, ParentComponent{parent});
    comp.add<TransformComponent>(child1, TransformComponent{});
    comp.add<HitboxComponent>(child1, HitboxComponent{});

    Entity child2 = this->world.entities().create();
    comp.add<LocalTransform>(child2, LocalTransform{{20.f, 0.f}});
    comp.add<ParentComponent>(child2, ParentComponent{parent});
    comp.add<TransformComponent>(child2, TransformComponent{});
    comp.add<HitboxComponent>(child2, HitboxComponent{});

    HitboxControllerComponent ctrl;
    ctrl.frames = {{{child1}, 0.1f}, {{child2}, 0.1f}};
    ctrl.currentFrame = 0;
    ctrl.initialized = true;
    comp.add<HitboxControllerComponent>(parent, std::move(ctrl));

    ICollisionController::ControllerParams params{parent, this->world};
    this->controller.onOrientationChanged(params);

    const auto& child1T = comp.get<TransformComponent>(child1);
    REQUIRE(child1T.position.x == Catch::Approx(10.f));
    REQUIRE(child1T.position.y == Catch::Approx(0.f));

    const auto& child2T = comp.get<TransformComponent>(child2);
    REQUIRE(child2T.position.x == Catch::Approx(0.f));
    REQUIRE(child2T.position.y == Catch::Approx(0.f));
}
