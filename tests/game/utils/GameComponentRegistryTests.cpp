#include "game/utils/ComponentRegistry/GameComponentRegistry.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/AirFrictionComponent.h"
#include "domain/components/AnalogInputComponent.h"
#include "domain/components/AnimationComponent.h"
#include "domain/components/AnimationControllerComponent.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/CircleColliderComponent.h"
#include "domain/components/CircleEffectsComponent.h"
#include "domain/components/CircleShapeComponent.h"
#include "domain/components/GravityComponent.h"
#include "domain/components/GroundedComponent.h"
#include "domain/components/HealthComponent.h"
#include "domain/components/HitboxComponent.h"
#include "domain/components/HitboxControllerComponent.h"
#include "domain/components/HitboxControllerMapComponent.h"
#include "domain/components/HitstopComponent.h"
#include "domain/components/HurtboxComponent.h"
#include "domain/components/HurtboxControllerComponent.h"
#include "domain/components/HurtboxControllerMapComponent.h"
#include "domain/components/InputBufferComponent.h"
#include "domain/components/InputComponent.h"
#include "domain/components/JumpComponent.h"
#include "domain/components/KnockbackComponent.h"
#include "domain/components/LifetimeComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/MapComponent.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParallaxComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/components/PushboxComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/components/PushboxControllerMapComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/RectangleEffectsComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/StateComponent.h"
#include "domain/components/StateMachineComponent.h"
#include "domain/components/TextureEffectsComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/ComponentManager/ComponentManager.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("GameComponentRegistry registers every game component", "[unit][component_registry]")
{
    ComponentManager comp;

    GameComponentRegistry::registerAll(comp);

    REQUIRE(comp.storage<ActiveComponent>() != nullptr);
    REQUIRE(comp.storage<AirFrictionComponent>() != nullptr);
    REQUIRE(comp.storage<AnalogInputComponent>() != nullptr);
    REQUIRE(comp.storage<AnimationComponent>() != nullptr);
    REQUIRE(comp.storage<AnimationControllerComponent>() != nullptr);
    REQUIRE(comp.storage<ChildrenComponent>() != nullptr);
    REQUIRE(comp.storage<CircleColliderComponent>() != nullptr);
    REQUIRE(comp.storage<CircleEffectsComponent>() != nullptr);
    REQUIRE(comp.storage<CircleShapeComponent>() != nullptr);
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
    REQUIRE(comp.storage<JumpComponent>() != nullptr);
    REQUIRE(comp.storage<KnockbackComponent>() != nullptr);
    REQUIRE(comp.storage<LifetimeComponent>() != nullptr);
    REQUIRE(comp.storage<LocalTransform>() != nullptr);
    REQUIRE(comp.storage<MapComponent>() != nullptr);
    REQUIRE(comp.storage<OrientationComponent>() != nullptr);
    REQUIRE(comp.storage<ParallaxComponent>() != nullptr);
    REQUIRE(comp.storage<ParentComponent>() != nullptr);
    REQUIRE(comp.storage<PlayerComponent>() != nullptr);
    REQUIRE(comp.storage<PushboxComponent>() != nullptr);
    REQUIRE(comp.storage<PushboxControllerComponent>() != nullptr);
    REQUIRE(comp.storage<PushboxControllerMapComponent>() != nullptr);
    REQUIRE(comp.storage<RectangleColliderComponent>() != nullptr);
    REQUIRE(comp.storage<RectangleEffectsComponent>() != nullptr);
    REQUIRE(comp.storage<RectangleShapeComponent>() != nullptr);
    REQUIRE(comp.storage<RenderComponent>() != nullptr);
    REQUIRE(comp.storage<SpriteComponent>() != nullptr);
    REQUIRE(comp.storage<StateComponent>() != nullptr);
    REQUIRE(comp.storage<StateMachineComponent>() != nullptr);
    REQUIRE(comp.storage<TextureEffectsComponent>() != nullptr);
    REQUIRE(comp.storage<TransformComponent>() != nullptr);
    REQUIRE(comp.storage<VelocityComponent>() != nullptr);
}

TEST_CASE("GameComponentRegistry can be invoked more than once safely", "[unit][component_registry]")
{
    ComponentManager comp;

    REQUIRE_NOTHROW(GameComponentRegistry::registerAll(comp));
    REQUIRE_NOTHROW(GameComponentRegistry::registerAll(comp));
}
