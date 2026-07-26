#include "game/utils/ComponentRegistry/ComponentRegistry.h"

#include "domain/components/AllComponents.h"
#include "domain/include/ComponentManager/ComponentManager.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("ComponentRegistry registers every game component", "[unit][component_registry]")
{
    ComponentManager comp;

    ComponentRegistry::registerAll(comp);

    REQUIRE(comp.storage<ActiveComponent>() != nullptr);
    REQUIRE(comp.storage<AirFrictionComponent>() != nullptr);
    REQUIRE(comp.storage<AnalogInputComponent>() != nullptr);
    REQUIRE(comp.storage<AnimationComponent>() != nullptr);
    REQUIRE(comp.storage<AnimationControllerComponent>() != nullptr);
    REQUIRE(comp.storage<BoxModel>() != nullptr);
    REQUIRE(comp.storage<CircleColliderComponent>() != nullptr);
    REQUIRE(comp.storage<CircleShapeComponent>() != nullptr);
    REQUIRE(comp.storage<FlexContainer>() != nullptr);
    REQUIRE(comp.storage<FlexItem>() != nullptr);
    REQUIRE(comp.storage<GravityComponent>() != nullptr);
    REQUIRE(comp.storage<GroundedComponent>() != nullptr);
    REQUIRE(comp.storage<HealthComponent>() != nullptr);
    REQUIRE(comp.storage<HitboxComponent>() != nullptr);
    REQUIRE(comp.storage<HitboxControllerComponent>() != nullptr);
    REQUIRE(comp.storage<HitboxControllerMapComponent>() != nullptr);
    REQUIRE(comp.storage<HitstopComponent>() != nullptr);
    REQUIRE(comp.storage<HurtboxComponent>() != nullptr);
    REQUIRE(comp.storage<HurtboxControllerComponent>() != nullptr);
    REQUIRE(comp.storage<HurtboxControllerMapComponent>() != nullptr);
    REQUIRE(comp.storage<InputBufferComponent>() != nullptr);
    REQUIRE(comp.storage<InputComponent>() != nullptr);
    REQUIRE(comp.storage<KnockbackComponent>() != nullptr);
    REQUIRE(comp.storage<LifetimeComponent>() != nullptr);
    REQUIRE(comp.storage<LocalTransform>() != nullptr);
    REQUIRE(comp.storage<MapComponent>() != nullptr);
    REQUIRE(comp.storage<OrientationComponent>() != nullptr);
    REQUIRE(comp.storage<OutlineComponent>() != nullptr);
    REQUIRE(comp.storage<ParallaxComponent>() != nullptr);
    REQUIRE(comp.storage<ParentComponent>() != nullptr);
    REQUIRE(comp.storage<PlayerComponent>() != nullptr);
    REQUIRE(comp.storage<PushboxComponent>() != nullptr);
    REQUIRE(comp.storage<PushboxControllerComponent>() != nullptr);
    REQUIRE(comp.storage<PushboxControllerMapComponent>() != nullptr);
    REQUIRE(comp.storage<RectangleColliderComponent>() != nullptr);
    REQUIRE(comp.storage<RectangleShapeComponent>() != nullptr);
    REQUIRE(comp.storage<RenderComponent>() != nullptr);
    REQUIRE(comp.storage<ShadowComponent>() != nullptr);
    REQUIRE(comp.storage<SpriteComponent>() != nullptr);
    REQUIRE(comp.storage<StateComponent>() != nullptr);
    REQUIRE(comp.storage<StateMachineComponent>() != nullptr);
    REQUIRE(comp.storage<TransformComponent>() != nullptr);
    REQUIRE(comp.storage<UIActionComponent>() != nullptr);
    REQUIRE(comp.storage<UIFocusable>() != nullptr);
    REQUIRE(comp.storage<UISpriteComponent>() != nullptr);
    REQUIRE(comp.storage<UITextComponent>() != nullptr);
    REQUIRE(comp.storage<UITransform>() != nullptr);
    REQUIRE(comp.storage<VelocityComponent>() != nullptr);
    REQUIRE(comp.storage<VisualEffectsComponent>() != nullptr);
}

TEST_CASE("ComponentRegistry can be invoked more than once safely", "[unit][component_registry]")
{
    ComponentManager comp;

    REQUIRE_NOTHROW(ComponentRegistry::registerAll(comp));
    REQUIRE_NOTHROW(ComponentRegistry::registerAll(comp));
}
