#include "../../src/game/include/ComponentRegistry/ComponentRegistry.h"

#include "../../src/domain/components/AllComponents.h"
#include "../../src/domain/include/ComponentManager/ComponentManager.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("ComponentRegistry registers every game component",
    "[unit][component_registry]"
) {
    ComponentManager components;

    ComponentRegistry::registerAll(components);

    REQUIRE(components.storage<AirFrictionComponent>() != nullptr);
    REQUIRE(components.storage<AnalogInputComponent>() != nullptr);
    REQUIRE(components.storage<AnimationComponent>() != nullptr);
    REQUIRE(components.storage<AnimationControllerComponent>() != nullptr);
    REQUIRE(components.storage<BoxModel>() != nullptr);
    REQUIRE(components.storage<CircleColliderComponent>() != nullptr);
    REQUIRE(components.storage<CollisionClipDefinitionsComponent>() != nullptr);
    REQUIRE(components.storage<CollisionClipPlayerComponent>() != nullptr);
    REQUIRE(components.storage<FlexContainer>() != nullptr);
    REQUIRE(components.storage<FlexItem>() != nullptr);
    REQUIRE(components.storage<GravityComponent>() != nullptr);
    REQUIRE(components.storage<GroundedComponent>() != nullptr);
    REQUIRE(components.storage<HealthComponent>() != nullptr);
    REQUIRE(components.storage<HitboxComponent>() != nullptr);
    REQUIRE(components.storage<HitstopComponent>() != nullptr);
    REQUIRE(components.storage<HurtboxComponent>() != nullptr);
    REQUIRE(components.storage<InputBufferComponent>() != nullptr);
    REQUIRE(components.storage<InputComponent>() != nullptr);
    REQUIRE(components.storage<KnockbackComponent>() != nullptr);
    REQUIRE(components.storage<LifetimeComponent>() != nullptr);
    REQUIRE(components.storage<LocalTransform>() != nullptr);
    REQUIRE(components.storage<OrientationComponent>() != nullptr);
    REQUIRE(components.storage<OutlineComponent>() != nullptr);
    REQUIRE(components.storage<ParallaxComponent>() != nullptr);
    REQUIRE(components.storage<ParentComponent>() != nullptr);
    REQUIRE(components.storage<PlayerComponent>() != nullptr);
    REQUIRE(components.storage<PushboxComponent>() != nullptr);
    REQUIRE(components.storage<RectangleColliderComponent>() != nullptr);
    REQUIRE(components.storage<RenderComponent>() != nullptr);
    REQUIRE(components.storage<ShadowComponent>() != nullptr);
    REQUIRE(components.storage<ShapeRenderComponent>() != nullptr);
    REQUIRE(components.storage<SpriteComponent>() != nullptr);
    REQUIRE(components.storage<StateComponent>() != nullptr);
    REQUIRE(components.storage<StateMappingComponent>() != nullptr);
    REQUIRE(components.storage<StateMachineComponent>() != nullptr);
    REQUIRE(components.storage<TransformComponent>() != nullptr);
    REQUIRE(components.storage<UIActionComponent>() != nullptr);
    REQUIRE(components.storage<UIFocusable>() != nullptr);
    REQUIRE(components.storage<UISpriteComponent>() != nullptr);
    REQUIRE(components.storage<UITextComponent>() != nullptr);
    REQUIRE(components.storage<UITransform>() != nullptr);
    REQUIRE(components.storage<VelocityComponent>() != nullptr);
    REQUIRE(components.storage<VisualEffectsComponent>() != nullptr);
}

TEST_CASE("ComponentRegistry can be invoked more than once safely",
    "[unit][component_registry]"
) {
    ComponentManager components;

    REQUIRE_NOTHROW(ComponentRegistry::registerAll(components));
    REQUIRE_NOTHROW(ComponentRegistry::registerAll(components));
}
