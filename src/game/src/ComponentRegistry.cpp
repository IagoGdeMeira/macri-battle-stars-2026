#include "../include/ComponentRegistry/ComponentRegistry.h"

#include "../../domain/components/AirFrictionComponent.h"
#include "../../domain/components/AnalogInputComponent.h"
#include "../../domain/components/AnimationComponent.h"
#include "../../domain/components/AnimationControllerComponent.h"
#include "../../domain/components/CollisionClipDefinitionsComponent.h"
#include "../../domain/components/CollisionClipPlayerComponent.h"
#include "../../domain/components/CircleColliderComponent.h"
#include "../../domain/components/GravityComponent.h"
#include "../../domain/components/GroundedComponent.h"
#include "../../domain/components/HealthComponent.h"
#include "../../domain/components/HitboxComponent.h"
#include "../../domain/components/HitstopComponent.h"
#include "../../domain/components/HurtboxComponent.h"
#include "../../domain/components/InputBufferComponent.h"
#include "../../domain/components/InputComponent.h"
#include "../../domain/components/KnockbackComponent.h"
#include "../../domain/components/LifetimeComponent.h"
#include "../../domain/components/LocalTransform.h"
#include "../../domain/components/OrientationComponent.h"
#include "../../domain/components/ParentComponent.h"
#include "../../domain/components/ParallaxComponent.h"
#include "../../domain/components/PlayerComponent.h"
#include "../../domain/components/PushboxComponent.h"
#include "../../domain/components/RectangleColliderComponent.h"
#include "../../domain/components/RenderComponent.h"
#include "../../domain/components/ShapeRenderComponent.h"
#include "../../domain/components/SpriteComponent.h"
#include "../../domain/components/StateComponent.h"
#include "../../domain/components/StateMappingComponent.h"
#include "../../domain/components/StateMachineComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/VelocityComponent.h"

#include "../../domain/include/ComponentManager/ComponentManager.h"

void ComponentRegistry::registerAll(ComponentManager& components)
{
    components.registerComponent<AirFrictionComponent>();
    components.registerComponent<AnalogInputComponent>();
    components.registerComponent<AnimationComponent>();
    components.registerComponent<AnimationControllerComponent>();
    components.registerComponent<CollisionClipDefinitionsComponent>();
    components.registerComponent<CollisionClipPlayerComponent>();
    components.registerComponent<CircleColliderComponent>();
    components.registerComponent<GravityComponent>();
    components.registerComponent<GroundedComponent>();
    components.registerComponent<HealthComponent>();
    components.registerComponent<HitboxComponent>();
    components.registerComponent<HitstopComponent>();
    components.registerComponent<HurtboxComponent>();
    components.registerComponent<InputBufferComponent>();
    components.registerComponent<InputComponent>();
    components.registerComponent<KnockbackComponent>();
    components.registerComponent<LifetimeComponent>();
    components.registerComponent<LocalTransform>();
    components.registerComponent<OrientationComponent>();
    components.registerComponent<ParallaxComponent>();
    components.registerComponent<ParentComponent>();
    components.registerComponent<PlayerComponent>();
    components.registerComponent<PushboxComponent>();
    components.registerComponent<RectangleColliderComponent>();
    components.registerComponent<RenderComponent>();
    components.registerComponent<ShapeRenderComponent>();
    components.registerComponent<SpriteComponent>();
    components.registerComponent<StateComponent>();
    components.registerComponent<StateMachineComponent>();
    components.registerComponent<StateMappingComponent>();
    components.registerComponent<TransformComponent>();
    components.registerComponent<VelocityComponent>();
}
