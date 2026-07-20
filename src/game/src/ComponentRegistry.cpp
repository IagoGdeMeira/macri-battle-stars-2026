#include "ComponentRegistry/ComponentRegistry.h"

#include "domain/components/AllComponents.h"
#include "domain/include/ComponentManager/ComponentManager.h"

void ComponentRegistry::registerAll(ComponentManager& components)
{
    components.registerComponent<AirFrictionComponent>();
    components.registerComponent<AnalogInputComponent>();
    components.registerComponent<AnimationComponent>();
    components.registerComponent<AnimationControllerComponent>();
    components.registerComponent<BoxModel>();
    components.registerComponent<CircleColliderComponent>();
    components.registerComponent<CollisionClipDefinitionsComponent>();
    components.registerComponent<CollisionClipPlayerComponent>();
    components.registerComponent<FlexContainer>();
    components.registerComponent<FlexItem>();
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
    components.registerComponent<MapComponent>();
    components.registerComponent<OrientationComponent>();
    components.registerComponent<OutlineComponent>();
    components.registerComponent<ParallaxComponent>();
    components.registerComponent<ParentComponent>();
    components.registerComponent<PlayerComponent>();
    components.registerComponent<PushboxComponent>();
    components.registerComponent<RectangleColliderComponent>();
    components.registerComponent<RenderComponent>();
    components.registerComponent<ShadowComponent>();
    components.registerComponent<ShapeRenderComponent>();
    components.registerComponent<SpriteComponent>();
    components.registerComponent<StateComponent>();
    components.registerComponent<StateMachineComponent>();
    components.registerComponent<TransformComponent>();
    components.registerComponent<UIActionComponent>();
    components.registerComponent<UIFocusable>();
    components.registerComponent<UISpriteComponent>();
    components.registerComponent<UITextComponent>();
    components.registerComponent<UITransform>();
    components.registerComponent<VelocityComponent>();
    components.registerComponent<VisualEffectsComponent>();
}
