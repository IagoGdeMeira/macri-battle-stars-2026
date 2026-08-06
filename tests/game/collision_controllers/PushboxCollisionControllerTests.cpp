#include "game/collision_controllers/PushboxCollisionController.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/PushboxComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/components/PushboxControllerMapComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class PushboxCollisionControllerFixture
{
public:
    PushboxCollisionControllerFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<ActiveComponent>();
        comp.registerComponent<PushboxControllerComponent>();
        comp.registerComponent<PushboxControllerMapComponent>();
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

TEST_CASE_METHOD(PushboxCollisionControllerFixture, "onOrientationChanged inverts pushbox position for active frame",
    "[unit][pushbox_collision_controller]")
{
    auto& comp = this->world.components();
    comp.registerComponent<TransformComponent>();
    comp.registerComponent<LocalTransform>();
    comp.registerComponent<ParentComponent>();
    comp.registerComponent<PushboxComponent>();
    comp.registerComponent<OrientationComponent>();

    Entity parent = this->world.entities().create();
    comp.add<TransformComponent>(parent, TransformComponent{100.f, 200.f});
    comp.add<OrientationComponent>(parent, OrientationComponent{Orientation::Left});

    Entity child = this->world.entities().create();
    comp.add<LocalTransform>(child, LocalTransform{{30.f, 10.f}});
    comp.add<ParentComponent>(child, ParentComponent{parent});
    comp.add<TransformComponent>(child, TransformComponent{});
    comp.add<PushboxComponent>(child, PushboxComponent{});

    PushboxControllerComponent ctrl;
    ctrl.frames = {{ {child}, 0.1f }};
    ctrl.initialized = true;
    comp.add<PushboxControllerComponent>(parent, std::move(ctrl));

    ICollisionController::ControllerParams params{parent, this->world};
    this->controller.onOrientationChanged(params);

    const auto& childT = comp.get<TransformComponent>(child);
    REQUIRE(childT.position.x == Catch::Approx(70.f));
    REQUIRE(childT.position.y == Catch::Approx(210.f));
}

TEST_CASE_METHOD(PushboxCollisionControllerFixture, "onOrientationChanged does nothing without controller component",
    "[unit][pushbox_collision_controller]")
{
    auto& comp = this->world.components();
    comp.registerComponent<OrientationComponent>();
    Entity parent = this->world.entities().create();
    comp.add<OrientationComponent>(parent, OrientationComponent{Orientation::Left});

    ICollisionController::ControllerParams params{parent, this->world};
    REQUIRE_NOTHROW(this->controller.onOrientationChanged(params));
}

TEST_CASE_METHOD(PushboxCollisionControllerFixture, "onOrientationChanged does nothing if controller not initialized",
    "[unit][pushbox_collision_controller]")
{
    auto& comp = this->world.components();
    comp.registerComponent<OrientationComponent>();
    Entity parent = this->world.entities().create();
    comp.add<OrientationComponent>(parent, OrientationComponent{Orientation::Left});
    comp.add<PushboxControllerComponent>(parent, PushboxControllerComponent{});

    ICollisionController::ControllerParams params{parent, this->world};
    REQUIRE_NOTHROW(this->controller.onOrientationChanged(params));
}

TEST_CASE_METHOD(PushboxCollisionControllerFixture, "onOrientationChanged only affects current frame entities",
    "[unit][pushbox_collision_controller]")
{
    auto& comp = this->world.components();
    comp.registerComponent<TransformComponent>();
    comp.registerComponent<LocalTransform>();
    comp.registerComponent<ParentComponent>();
    comp.registerComponent<PushboxComponent>();
    comp.registerComponent<OrientationComponent>();

    Entity parent = this->world.entities().create();
    comp.add<TransformComponent>(parent, TransformComponent{});
    comp.add<OrientationComponent>(parent, OrientationComponent{Orientation::Right});

    Entity child1 = this->world.entities().create();
    comp.add<LocalTransform>(child1, LocalTransform{{10.f, 0.f}});
    comp.add<ParentComponent>(child1, ParentComponent{parent});
    comp.add<TransformComponent>(child1, TransformComponent{});
    comp.add<PushboxComponent>(child1, PushboxComponent{});

    Entity child2 = this->world.entities().create();
    comp.add<LocalTransform>(child2, LocalTransform{{20.f, 0.f}});
    comp.add<ParentComponent>(child2, ParentComponent{parent});
    comp.add<TransformComponent>(child2, TransformComponent{});
    comp.add<PushboxComponent>(child2, PushboxComponent{});

    PushboxControllerComponent ctrl;
    ctrl.frames = {{{child1}, 0.1f}, {{child2}, 0.1f}};
    ctrl.currentFrame = 0;
    ctrl.initialized = true;
    comp.add<PushboxControllerComponent>(parent, std::move(ctrl));

    ICollisionController::ControllerParams params{parent, this->world};
    this->controller.onOrientationChanged(params);

    const auto& child1T = comp.get<TransformComponent>(child1);
    REQUIRE(child1T.position.x == Catch::Approx(10.f));
    REQUIRE(child1T.position.y == Catch::Approx(0.f));

    const auto& child2T = comp.get<TransformComponent>(child2);
    REQUIRE(child2T.position.x == Catch::Approx(0.f));
    REQUIRE(child2T.position.y == Catch::Approx(0.f));
}
