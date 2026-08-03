#include "ComponentRegistry/ComponentRegistry.h"

#include "domain/components/AllComponents.h"
#include "domain/include/ComponentManager/ComponentManager.h"

void ComponentRegistry::registerAll(ComponentManager& comp)
{
    comp.registerComponent<ActiveComponent>();
    comp.registerComponent<AirFrictionComponent>();
    comp.registerComponent<AnalogInputComponent>();
    comp.registerComponent<AnimationComponent>();
    comp.registerComponent<AnimationControllerComponent>();
    comp.registerComponent<BoxModel>();
    comp.registerComponent<ChildrenComponent>();
    comp.registerComponent<CircleColliderComponent>();
    comp.registerComponent<CircleShapeComponent>();
    comp.registerComponent<FlexContainer>();
    comp.registerComponent<FlexItem>();
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
    comp.registerComponent<KnockbackComponent>();
    comp.registerComponent<LifetimeComponent>();
    comp.registerComponent<LocalTransform>();
    comp.registerComponent<MapComponent>();
    comp.registerComponent<OrientationComponent>();
    comp.registerComponent<OutlineComponent>();
    comp.registerComponent<ParallaxComponent>();
    comp.registerComponent<ParentComponent>();
    comp.registerComponent<PlayerComponent>();
    comp.registerComponent<PushboxComponent>();
    comp.registerComponent<PushboxControllerComponent>();
    comp.registerComponent<PushboxControllerMapComponent>();
    comp.registerComponent<RectangleColliderComponent>();
    comp.registerComponent<RectangleShapeComponent>();
    comp.registerComponent<RenderComponent>();
    comp.registerComponent<ShadowComponent>();
    comp.registerComponent<SpriteComponent>();
    comp.registerComponent<StateComponent>();
    comp.registerComponent<StateMachineComponent>();
    comp.registerComponent<TransformComponent>();
    comp.registerComponent<UIActionComponent>();
    comp.registerComponent<UIFocusable>();
    comp.registerComponent<UISpriteComponent>();
    comp.registerComponent<UITextComponent>();
    comp.registerComponent<UITransform>();
    comp.registerComponent<VelocityComponent>();
    comp.registerComponent<VisualEffectsComponent>();
}
