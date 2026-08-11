#include "ComponentRegistry/GameComponentRegistry.h"

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

void GameComponentRegistry::registerAll(ComponentManager& comp)
{
    comp.registerComponent<ActiveComponent>();
    comp.registerComponent<AirFrictionComponent>();
    comp.registerComponent<AnalogInputComponent>();
    comp.registerComponent<AnimationComponent>();
    comp.registerComponent<AnimationControllerComponent>();
    comp.registerComponent<ChildrenComponent>();
    comp.registerComponent<CircleColliderComponent>();
    comp.registerComponent<CircleEffectsComponent>();
    comp.registerComponent<CircleShapeComponent>();
    comp.registerComponent<GravityComponent>();
    comp.registerComponent<GroundedComponent>();
    comp.registerComponent<HealthComponent>();
    comp.registerComponent<HitboxComponent>();
    comp.registerComponent<HitboxControllerComponent>();
    comp.registerComponent<HitboxControllerMapComponent>();
    comp.registerComponent<HitstopComponent>();
    comp.registerComponent<HurtboxComponent>();
    comp.registerComponent<HurtboxControllerComponent>();
    comp.registerComponent<HurtboxControllerMapComponent>();
    comp.registerComponent<InputBufferComponent>();
    comp.registerComponent<InputComponent>();
    comp.registerComponent<JumpComponent>();
    comp.registerComponent<KnockbackComponent>();
    comp.registerComponent<LifetimeComponent>();
    comp.registerComponent<LocalTransform>();
    comp.registerComponent<MapComponent>();
    comp.registerComponent<OrientationComponent>();
    comp.registerComponent<ParallaxComponent>();
    comp.registerComponent<ParentComponent>();
    comp.registerComponent<PlayerComponent>();
    comp.registerComponent<PushboxComponent>();
    comp.registerComponent<PushboxControllerComponent>();
    comp.registerComponent<PushboxControllerMapComponent>();
    comp.registerComponent<RectangleColliderComponent>();
    comp.registerComponent<RectangleEffectsComponent>();
    comp.registerComponent<RectangleShapeComponent>();
    comp.registerComponent<RenderComponent>();
    comp.registerComponent<SpriteComponent>();
    comp.registerComponent<StateComponent>();
    comp.registerComponent<StateMachineComponent>();
    comp.registerComponent<TextureEffectsComponent>();
    comp.registerComponent<TransformComponent>();
    comp.registerComponent<VelocityComponent>();
}
