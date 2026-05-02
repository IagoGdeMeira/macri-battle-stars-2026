#include "../include/EntityFactory/EntityFactory.h"

#include "../../domain/include/World/World.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/RectangleColliderComponent.h"
#include "../../domain/components/CircleColliderComponent.h"
#include "../../domain/components/SpriteComponent.h"
#include "../../domain/components/ParallaxComponent.h"
#include "../../domain/components/RenderComponent.h"
#include "../../domain/components/ParentComponent.h"
#include "../../domain/components/LocalTransform.h"
#include "../../domain/components/HitboxComponent.h"
#include "../../domain/components/HurtboxComponent.h"
#include "../../domain/components/LifetimeComponent.h"
#include "../../domain/components/PushboxComponent.h"
#include "../../domain/components/ShapeRenderComponent.h"
#include "../../domain/include/CollisionFrame/ColliderDef.h"
#include "../../domain/include/CollisionFrame/CollisionFrame.h"

#include "../../engine/include/ResourceManager/ResourceManager.h"
#include "../../engine/include/Texture/Texture.h"
#include "../../engine/include/TextureLoader/TextureLoader.h"

EntityFactory::EntityFactory(World& world, ResourceManager& resourceManager, TextureLoader& textureLoader) :
    world(world), resourceManager(resourceManager), textureLoader(textureLoader) {}

Entity EntityFactory::createStaticBody(const Position& position)
{
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();
    comp.add<TransformComponent>(e, TransformComponent{position.x, position.y});
    return e;
}

Entity EntityFactory::addStaticCollider(Entity parent, const Rectangle& rect)
{
    Entity collider = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<ParentComponent>(collider, ParentComponent{parent});
    comp.add<LocalTransform>(collider, LocalTransform{rect.position.x, rect.position.y});
    comp.add<RectangleColliderComponent>(collider, RectangleColliderComponent{rect.width, rect.height});
    comp.add<PushboxComponent>(collider, PushboxComponent{PushboxComponent::PushboxType::Static});

    ColliderDebugDef staticDebug;
    staticDebug.color = {128, 128, 128, 128};
    staticDebug.enabled = true;
    
    RectangleDef rectDef;
    rectDef.width = rect.width;
    rectDef.height = rect.height;
    this->addDebugVisual(collider, rectDef, staticDebug);

    return collider;
}

Entity EntityFactory::addStaticCollider(Entity parent, const Circle& circle)
{
    Entity collider = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<ParentComponent>(collider, ParentComponent{parent});
    comp.add<LocalTransform>(collider, LocalTransform{circle.position.x, circle.position.y});
    comp.add<CircleColliderComponent>(collider, CircleColliderComponent{circle.radius});
    comp.add<PushboxComponent>(collider, PushboxComponent{PushboxComponent::PushboxType::Static});

    ColliderDebugDef staticDebug;
    staticDebug.color = {128, 128, 128, 128};
    staticDebug.enabled = true;

    CircleDef circleDef;
    circleDef.radius = circle.radius;
    this->addDebugVisual(collider, circleDef, staticDebug);

    return collider;
}

Entity EntityFactory::createBackgroundLayer(const BackgroundLayer& layer)
{
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();

    auto texture = this->resourceManager.load<Texture>(this->textureLoader, layer.texturePath);

    SpriteComponent sprite;
    sprite.texture = texture;
    sprite.width = texture->getWidth();
    sprite.height = texture->getHeight();
    sprite.useSourceRect = false;
    comp.add<SpriteComponent>(e, std::move(sprite));

    comp.add<ParallaxComponent>(e, ParallaxComponent{layer.parallaxFactorX, layer.parallaxFactorY});
    comp.add<RenderComponent>(e, RenderComponent{0, layer.zIndex});

    return e;
}

void EntityFactory::addColliderComponents(Entity entity, const ColliderDef& collider)
{
    auto& comp = this->world.components();

    switch (collider.getType())
    {
        case ColliderDef::ColliderType::Rectangle:
        {
            auto& rect = static_cast<const RectangleDef&>(collider);
            comp.add<RectangleColliderComponent>(entity, RectangleColliderComponent{rect.width, rect.height});
            break;
        }
        case ColliderDef::ColliderType::Circle:
        {
            auto& circ = static_cast<const CircleDef&>(collider);
            comp.add<CircleColliderComponent>(entity, CircleColliderComponent{circ.radius});
            break;
        }
    }
}

void EntityFactory::addDebugVisual(Entity entity, const ColliderDef& collider, const ColliderDebugDef& debug)
{
    if (!debug.enabled) return;

    auto& comp = this->world.components();
    ShapeRenderComponent shape;
    shape.shape = collider.clone();
    shape.color = {debug.color.r, debug.color.g, debug.color.b, debug.color.a};
    shape.filled = false;
    shape.layer = 10;
    comp.add<ShapeRenderComponent>(entity, std::move(shape));
}

Entity EntityFactory::createPushbox(Entity parent, const PushboxDef& def)
{
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<ParentComponent>(e, ParentComponent{parent});
    comp.add<LocalTransform>(e, LocalTransform{def.collider->offsetX, def.collider->offsetY});
    this->addColliderComponents(e, *def.collider);
    comp.add<PushboxComponent>(e, PushboxComponent{def.type, def.mass, def.pushResistance});
    this->addDebugVisual(e, *def.collider, def.debug);

    return e;
}

Entity EntityFactory::createHitbox(Entity parent, const HitboxDef& def)
{
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<ParentComponent>(e, ParentComponent{parent});
    comp.add<LocalTransform>(e, LocalTransform{def.collider->offsetX, def.collider->offsetY});
    this->addColliderComponents(e, *def.collider);
    comp.add<HitboxComponent>(e, HitboxComponent{def.damage});
    this->addDebugVisual(e, *def.collider, def.debug);

    return e;
}

Entity EntityFactory::createHurtbox(Entity parent, const HurtboxDef& def)
{
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<ParentComponent>(e, ParentComponent{parent});
    comp.add<LocalTransform>(e, LocalTransform{def.collider->offsetX, def.collider->offsetY});
    this->addColliderComponents(e, *def.collider);
    comp.add<HurtboxComponent>(e, HurtboxComponent{def.damageMultiplier});
    this->addDebugVisual(e, *def.collider, def.debug);

    return e;
}

Entity EntityFactory::createSpriteEffect(const std::string& texturePath, const Position& position, float duration)
{
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();

    auto texture = this->resourceManager.load<Texture>(this->textureLoader, texturePath);

    SpriteComponent sprite;
    sprite.texture = texture;
    sprite.width = texture->getWidth();
    sprite.height = texture->getHeight();
    comp.add<SpriteComponent>(e, std::move(sprite));

    comp.add<TransformComponent>(e, TransformComponent{position.x, position.y});
    comp.add<LifetimeComponent>(e, LifetimeComponent{duration});

    return e;
}
