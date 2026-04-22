#include "../include/ComponentRegistry/ComponentRegistry.h"

#include "../../domain/components/AirFrictionComponent.h"
#include "../../domain/components/AnimationComponent.h"
#include "../../domain/components/AnimationControllerComponent.h"
#include "../../domain/components/CircleColliderComponent.h"
#include "../../domain/components/GravityComponent.h"
#include "../../domain/components/GroundedComponent.h"
#include "../../domain/components/HealthComponent.h"
#include "../../domain/components/HitboxComponent.h"
#include "../../domain/components/HurtboxComponent.h"
#include "../../domain/components/InputBufferComponent.h"
#include "../../domain/components/InputComponent.h"
#include "../../domain/components/LocalTransform.h"
#include "../../domain/components/ParentComponent.h"
#include "../../domain/components/ParallaxComponent.h"
#include "../../domain/components/PlayerComponent.h"
#include "../../domain/components/RectangleColliderComponent.h"
#include "../../domain/components/RenderComponent.h"
#include "../../domain/components/SpriteComponent.h"
#include "../../domain/components/StateComponent.h"
#include "../../domain/components/StateMappingComponent.h"
#include "../../domain/components/StateMachineComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/VelocityComponent.h"

#include "../../domain/include/ComponentManager/ComponentManager.h"

void ComponentRegistry::registerAll(ComponentManager& components)
{
    components.registerComponent<PlayerComponent>();
    components.registerComponent<InputComponent>();
    components.registerComponent<InputBufferComponent>();
    components.registerComponent<StateComponent>();
    components.registerComponent<StateMappingComponent>();
    components.registerComponent<StateMachineComponent>();
    components.registerComponent<TransformComponent>();
    components.registerComponent<VelocityComponent>();
    components.registerComponent<GroundedComponent>();
    components.registerComponent<GravityComponent>();
    components.registerComponent<AirFrictionComponent>();
    components.registerComponent<RectangleColliderComponent>();
    components.registerComponent<CircleColliderComponent>();
    components.registerComponent<SpriteComponent>();
    components.registerComponent<AnimationComponent>();
    components.registerComponent<AnimationControllerComponent>();
    components.registerComponent<ParallaxComponent>();
    components.registerComponent<RenderComponent>();
    components.registerComponent<ParentComponent>();
    components.registerComponent<LocalTransform>();
    components.registerComponent<HitboxComponent>();
    components.registerComponent<HurtboxComponent>();
    components.registerComponent<HealthComponent>();
}
