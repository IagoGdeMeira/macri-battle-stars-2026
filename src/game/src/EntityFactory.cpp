#include "EntityFactory/EntityFactory.h"

#include "domain/include/World/World.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/CircleColliderComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/ParallaxComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/HitboxComponent.h"
#include "domain/components/HurtboxComponent.h"
#include "domain/components/LifetimeComponent.h"
#include "domain/components/PushboxComponent.h"
#include "domain/components/ShapeRenderComponent.h"
#include "domain/utils/Logger/Logger.h"
#include "domain/value_objects/CollisionFrame/ColliderDef.h"
#include "domain/value_objects/CollisionFrame/CollisionFrame.h"

#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/TextureLoader/TextureLoader.h"

Entity EntityFactory::createStaticEntity(const Position& position, const Rectangle& rect, std::optional<Entity> parent)
{
    RectangleDef rectDef;
    rectDef.offset = {0.f, 0.f};
    rectDef.width = rect.size.width;
    rectDef.height = rect.size.height;
    return this->createStaticEntityImpl(position, rectDef, parent);
}

Entity EntityFactory::createStaticEntity(const Position& position, const Circle& circle, std::optional<Entity> parent)
{
    CircleDef circleDef;
    circleDef.offset = {0.f, 0.f};
    circleDef.radius = circle.radius;
    return this->createStaticEntityImpl(position, circleDef, parent);
}

Entity EntityFactory::createPushbox(Entity parent, const PushboxDef& def, bool facingLeft)
{
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();

    float finalOffsetX = facingLeft ? -def.collider->offset.x : def.collider->offset.x;
    comp.add<ParentComponent>(e, ParentComponent{parent});
    comp.add<LocalTransform>(e, LocalTransform{finalOffsetX, def.collider->offset.y});
    this->addColliderComponents(e, *def.collider);
    comp.add<PushboxComponent>(e, PushboxComponent{def.type, def.mass, def.pushResistance});
    this->addDebugVisual(e, *def.collider, def.debug);

    return e;
}

Entity EntityFactory::createHitbox(Entity parent, const HitboxDef& def, bool facingLeft)
{
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();

    float finalOffsetX = facingLeft ? -def.collider->offset.x : def.collider->offset.x;
    comp.add<ParentComponent>(e, ParentComponent{parent});
    comp.add<LocalTransform>(e, LocalTransform{finalOffsetX, def.collider->offset.y});
    this->addColliderComponents(e, *def.collider);
    comp.add<HitboxComponent>(e, HitboxComponent{def.damage});
    this->addDebugVisual(e, *def.collider, def.debug);

    return e;
}

Entity EntityFactory::createHurtbox(Entity parent, const HurtboxDef& def, bool facingLeft)
{
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();

    float finalOffsetX = facingLeft ? -def.collider->offset.x : def.collider->offset.x;
    comp.add<ParentComponent>(e, ParentComponent{parent});
    comp.add<LocalTransform>(e, LocalTransform{finalOffsetX, def.collider->offset.y});
    this->addColliderComponents(e, *def.collider);
    comp.add<HurtboxComponent>(e, HurtboxComponent{def.damageMultiplier});
    this->addDebugVisual(e, *def.collider, def.debug);

    return e;
}

Entity EntityFactory::createSpriteEffect(const std::string& texturePath, const Position& position, float duration)
{
    Entity e = this->world.entities().create();
    this->addSprite(e, texturePath);
    auto& comp = this->world.components();
    comp.add<TransformComponent>(e, TransformComponent{position.x, position.y});
    comp.add<LifetimeComponent>(e, LifetimeComponent{duration});
    return e;
}

Entity EntityFactory::createBackgroundChild(
    const std::string& texturePath,
    const Position& parallax,
    int zIndex,
    Entity parent
) {
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();

    this->addSprite(e, texturePath);
    this->addRender(e, 0, zIndex);
    this->addParallax(e, parallax);
    this->addParentAndLocal(e, parent, {0.f, 0.f});
    comp.add<TransformComponent>(e, TransformComponent{0.f, 0.f});

    LOG_INFO("Background entity {} created with parallax ({}, {})", e.id, parallax.x, parallax.y);
    return e;
}

Entity EntityFactory::createFloorChild(
    const std::string& texturePath,
    const Position& position,
    const Dimension2D& size,
    Entity parent
) {
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();

    this->addParentAndLocal(e, parent, position);
    comp.add<RectangleColliderComponent>(e, RectangleColliderComponent{size.width, size.height});

    if (!texturePath.empty())
    {
        this->addSprite(e, texturePath);
        comp.get<SpriteComponent>(e).size = size;
        this->addRender(e, 0, 0);
    }

    comp.add<TransformComponent>(e, TransformComponent{0.f, 0.f});

    return e;
}

Entity EntityFactory::createWallChild(const Position& position, const Dimension2D& size, Entity parent)
{
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();

    this->addParentAndLocal(e, parent, position);
    comp.add<RectangleColliderComponent>(e, RectangleColliderComponent{size.width, size.height});
    comp.add<TransformComponent>(e, TransformComponent{0.f, 0.f});

    return e;
}

Entity EntityFactory::createStaticEntityImpl(const Position& position, const ColliderDef& colliderDef, std::optional<Entity> parent)
{
    auto& entities = this->world.entities();
    auto& comp = this->world.components();

    Entity e = entities.create();

    if (parent.has_value() && entities.isAlive(parent.value()))
    {
        Entity p = parent.value();

        comp.add<ParentComponent>(e, ParentComponent{p});
        comp.add<LocalTransform>(e, LocalTransform{position.x, position.y});
        comp.add<TransformComponent>(e, TransformComponent{0.f, 0.f});
    }
    else comp.add<TransformComponent>(e, TransformComponent{position.x, position.y});

    this->addColliderComponents(e, colliderDef);
    comp.add<PushboxComponent>(e, PushboxComponent{PushboxComponent::Type::Static});

    ColliderDebugDef debug;
    debug.color = {128, 128, 128, 128};
    debug.enabled = true;
    this->addDebugVisual(e, colliderDef, debug);

    return e;
}

void EntityFactory::addColliderComponents(Entity entity, const ColliderDef& collider)
{
    auto& comp = this->world.components();

    using CType = ColliderDef::ColliderType;
    switch (collider.getType())
    {
        case CType::Rectangle:
        {
            auto& rect = static_cast<const RectangleDef&>(collider);
            comp.add<RectangleColliderComponent>(entity, RectangleColliderComponent{rect.width, rect.height});
            break;
        }
        case CType::Circle:
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

void EntityFactory::addSprite(Entity entity, const std::string& texturePath)
{
    auto texture = this->resourceManager.load<Texture>(this->textureLoader, texturePath);
    LOG_DEBUG("addSprite: texture loaded, width={}, height={}", texture->getWidth(), texture->getHeight());
    SpriteComponent sprite;
    sprite.texturePath = texturePath;
    sprite.size = {0.f, 0.f};
    sprite.useSourceRect = false;
    this->world.components().add<SpriteComponent>(entity, std::move(sprite));
}

void EntityFactory::addRender(Entity entity, int layer, int zIndex)
{ this->world.components().add<RenderComponent>(entity, RenderComponent{layer, zIndex}); }

void EntityFactory::addParallax(Entity entity, const Position& factor)
{ this->world.components().add<ParallaxComponent>(entity, ParallaxComponent{factor}); }

void EntityFactory::addParentAndLocal(Entity entity, Entity parent, const Position& localPos)
{
    auto& comp = this->world.components();
    comp.add<ParentComponent>(entity, ParentComponent{parent});
    comp.add<LocalTransform>(entity, LocalTransform{localPos});
}