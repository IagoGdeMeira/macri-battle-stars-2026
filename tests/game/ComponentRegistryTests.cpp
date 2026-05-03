#include "../../src/game/include/ComponentRegistry/ComponentRegistry.h"

#include "../../src/domain/components/AirFrictionComponent.h"
#include "../../src/domain/components/AnalogInputComponent.h"
#include "../../src/domain/components/AnimationComponent.h"
#include "../../src/domain/components/AnimationControllerComponent.h"
#include "../../src/domain/components/CircleColliderComponent.h"
#include "../../src/domain/components/GravityComponent.h"
#include "../../src/domain/components/GroundedComponent.h"
#include "../../src/domain/components/HealthComponent.h"
#include "../../src/domain/components/HitboxComponent.h"
#include "../../src/domain/components/HurtboxComponent.h"
#include "../../src/domain/components/InputBufferComponent.h"
#include "../../src/domain/components/InputComponent.h"
#include "../../src/domain/components/LifetimeComponent.h"
#include "../../src/domain/components/LocalTransform.h"
#include "../../src/domain/components/OrientationComponent.h"
#include "../../src/domain/components/ParallaxComponent.h"
#include "../../src/domain/components/ParentComponent.h"
#include "../../src/domain/components/PlayerComponent.h"
#include "../../src/domain/components/PushboxComponent.h"
#include "../../src/domain/components/RectangleColliderComponent.h"
#include "../../src/domain/components/RenderComponent.h"
#include "../../src/domain/components/ShapeRenderComponent.h"
#include "../../src/domain/components/SpriteComponent.h"
#include "../../src/domain/components/StateComponent.h"
#include "../../src/domain/components/StateMappingComponent.h"
#include "../../src/domain/components/StateMachineComponent.h"
#include "../../src/domain/components/TransformComponent.h"
#include "../../src/domain/components/VelocityComponent.h"

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
    REQUIRE(components.storage<CircleColliderComponent>() != nullptr);
    REQUIRE(components.storage<GravityComponent>() != nullptr);
    REQUIRE(components.storage<GroundedComponent>() != nullptr);
    REQUIRE(components.storage<HealthComponent>() != nullptr);
    REQUIRE(components.storage<HitboxComponent>() != nullptr);
    REQUIRE(components.storage<HurtboxComponent>() != nullptr);
    REQUIRE(components.storage<InputBufferComponent>() != nullptr);
    REQUIRE(components.storage<InputComponent>() != nullptr);
    REQUIRE(components.storage<LifetimeComponent>() != nullptr);
    REQUIRE(components.storage<LocalTransform>() != nullptr);
    REQUIRE(components.storage<OrientationComponent>() != nullptr);
    REQUIRE(components.storage<ParallaxComponent>() != nullptr);
    REQUIRE(components.storage<ParentComponent>() != nullptr);
    REQUIRE(components.storage<PlayerComponent>() != nullptr);
    REQUIRE(components.storage<PushboxComponent>() != nullptr);
    REQUIRE(components.storage<RectangleColliderComponent>() != nullptr);
    REQUIRE(components.storage<RenderComponent>() != nullptr);
    REQUIRE(components.storage<ShapeRenderComponent>() != nullptr);
    REQUIRE(components.storage<SpriteComponent>() != nullptr);
    REQUIRE(components.storage<StateComponent>() != nullptr);
    REQUIRE(components.storage<StateMappingComponent>() != nullptr);
    REQUIRE(components.storage<StateMachineComponent>() != nullptr);
    REQUIRE(components.storage<TransformComponent>() != nullptr);
    REQUIRE(components.storage<VelocityComponent>() != nullptr);
}

TEST_CASE("ComponentRegistry can be invoked more than once safely",
    "[unit][component_registry]"
) {
    ComponentManager components;

    REQUIRE_NOTHROW(ComponentRegistry::registerAll(components));
    REQUIRE_NOTHROW(ComponentRegistry::registerAll(components));
}
