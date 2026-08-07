#include "EntityFactory/EntityFactory.h"

#include "StateIdMapper/StateIdMapper.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/AnimationComponent.h"
#include "domain/components/AnimationControllerComponent.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/CircleColliderComponent.h"
#include "domain/components/CircleShapeComponent.h"
#include "domain/components/HitboxComponent.h"
#include "domain/components/HurtboxComponent.h"
#include "domain/components/LifetimeComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/ParallaxComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/PushboxComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/StateId/StateId.h"

#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/TextureLoader/TextureLoader.h"

Entity EntityFactory::createHitboxChild(const HitboxChildParams& params, const Rectangle& rect)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    this->addParentAndLocal(e, params.parent, params.offset);
    comp.add<TransformComponent>(e, TransformComponent{});
    comp.add<ActiveComponent>(e, ActiveComponent{false});
    if (params.debug.enabled) comp.add<RenderComponent>(e, RenderComponent{params.debug.layer, params.debug.zIndex});
    this->addCollider(e, rect);
    comp.add<HitboxComponent>(e, HitboxComponent{params.damage});

    if (params.debug.enabled) this->addDebugVisual(e, rect, params.debug);
    return e;
}

Entity EntityFactory::createHitboxChild(const HitboxChildParams& params, const Circle& circle)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    this->addParentAndLocal(e, params.parent, params.offset);
    comp.add<TransformComponent>(e, TransformComponent{});
    comp.add<ActiveComponent>(e, ActiveComponent{false});
    if (params.debug.enabled) comp.add<RenderComponent>(e, RenderComponent{params.debug.layer, params.debug.zIndex});
    this->addCollider(e, circle);
    comp.add<HitboxComponent>(e, HitboxComponent{params.damage});

    if (params.debug.enabled) this->addDebugVisual(e, circle, params.debug);
    return e;
}

Entity EntityFactory::createHurtboxChild(const HurtboxChildParams& params, const Rectangle& rect)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    this->addParentAndLocal(e, params.parent, params.offset);
    comp.add<TransformComponent>(e, TransformComponent{});
    comp.add<ActiveComponent>(e, ActiveComponent{false});
    if (params.debug.enabled) comp.add<RenderComponent>(e, RenderComponent{params.debug.layer, params.debug.zIndex});
    this->addCollider(e, rect);
    comp.add<HurtboxComponent>(e, HurtboxComponent{params.damageMultiplier});

    if (params.debug.enabled) this->addDebugVisual(e, rect, params.debug);
    return e;
}

Entity EntityFactory::createHurtboxChild(const HurtboxChildParams& params, const Circle& circle)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    this->addParentAndLocal(e, params.parent, params.offset);
    comp.add<TransformComponent>(e, TransformComponent{});
    comp.add<ActiveComponent>(e, ActiveComponent{false});
    if (params.debug.enabled) comp.add<RenderComponent>(e, RenderComponent{params.debug.layer, params.debug.zIndex});
    this->addCollider(e, circle);
    comp.add<HurtboxComponent>(e, HurtboxComponent{params.damageMultiplier});

    if (params.debug.enabled) this->addDebugVisual(e, circle, params.debug);
    return e;
}

Entity EntityFactory::createPushboxChild(const PushboxChildParams& params, const Rectangle& rect)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    this->addParentAndLocal(e, params.parent, params.offset);
    comp.add<TransformComponent>(e, TransformComponent{});
    comp.add<ActiveComponent>(e, ActiveComponent{false});
    if (params.debug.enabled) comp.add<RenderComponent>(e, RenderComponent{params.debug.layer, params.debug.zIndex});
    this->addCollider(e, rect);
    comp.add<PushboxComponent>(e, PushboxComponent{params.type, params.mass, params.pushResistance});

    if (params.debug.enabled) this->addDebugVisual(e, rect, params.debug);
    return e;
}

Entity EntityFactory::createPushboxChild(const PushboxChildParams& params, const Circle& circle)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    this->addParentAndLocal(e, params.parent, params.offset);
    comp.add<TransformComponent>(e, TransformComponent{});
    comp.add<ActiveComponent>(e, ActiveComponent{false});
    if (params.debug.enabled) comp.add<RenderComponent>(e, RenderComponent{params.debug.layer, params.debug.zIndex});
    this->addCollider(e, circle);
    comp.add<PushboxComponent>(e, PushboxComponent{params.type, params.mass, params.pushResistance});

    if (params.debug.enabled) this->addDebugVisual(e, circle, params.debug);
    return e;
}

Entity EntityFactory::createEffectSprite(const std::string& texturePath, const Position& position, float duration)
{
    Entity e = this->factoryWorld.entities().create();
    this->addSprite(e, texturePath);
    auto& comp = this->factoryWorld.components();
    comp.add<TransformComponent>(e, TransformComponent{position.x, position.y});
    comp.add<LifetimeComponent>(e, LifetimeComponent{duration});
    return e;
}

Entity EntityFactory::createBackgroundSprite(const BackgroundParams& params, const std::string& texturePath)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    this->addSprite(e, texturePath);
    this->addRender(e, 0, params.zIndex);
    this->addParallax(e, params.parallax);
    this->addParentAndLocal(e, params.parent, {0.f, 0.f});
    comp.add<TransformComponent>(e, TransformComponent{0.f, 0.f});

    return e;
}

Entity EntityFactory::createBackgroundRectangle(const BackgroundParams& params, const Rectangle& rect, const Color& color, bool filled)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    this->addParentAndLocal(e, params.parent, {0.f, 0.f});
    comp.add<TransformComponent>(e, TransformComponent{0.f, 0.f});
    this->addParallax(e, params.parallax);
    this->addRender(e, 0, params.zIndex);

    this->addRectangleShape(ShapeParams{e, color, filled, 0}, rect);
    return e;
}

Entity EntityFactory::createBackgroundCircle(const BackgroundParams& params, const Circle& circle, const Color& color, bool filled)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    this->addParentAndLocal(e, params.parent, {0.f, 0.f});
    comp.add<TransformComponent>(e, TransformComponent{0.f, 0.f});
    this->addParallax(e, params.parallax);
    this->addRender(e, 0, params.zIndex);

    this->addCircleShape(ShapeParams{e, color, filled, 0}, circle);
    return e;
}

Entity EntityFactory::createBackgroundAnimated(const BackgroundParams& params, const std::string& texturePath, const std::string& animationPath)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    this->addParentAndLocal(e, params.parent, {0.f, 0.f});
    comp.add<TransformComponent>(e, TransformComponent{0.f, 0.f});
    this->addParallax(e, params.parallax);
    this->addRender(e, 0, params.zIndex);

    this->addSprite(e, texturePath);

    AnimationControllerComponent controller;
    StateIdMapper defaultMapper;
    controller.animations = this->animLoader.loadFromIndex(animationPath, defaultMapper);
    controller.currentState = StateId::Idle;
    comp.add<AnimationControllerComponent>(e, std::move(controller));

    AnimationComponent anim;
    anim.currentState   = StateId::Idle;
    anim.elapsedTime    = 0.f;
    anim.currentFrame   = 0;
    
    auto& anims = comp.get<AnimationControllerComponent>(e).animations;
    auto it = anims.right.find(StateId::Idle);
    if (it != anims.right.end()) anim.animation = it->second;
    comp.add<AnimationComponent>(e, std::move(anim));

    return e;
}

void EntityFactory::addParentAndLocal(Entity entity, Entity parent, const Position& localPos)
{
    auto& comp = this->factoryWorld.components();
    comp.add<ParentComponent>(entity, ParentComponent{parent});
    comp.add<LocalTransform>(entity, LocalTransform{localPos});

    if (!comp.has<ChildrenComponent>(parent)) comp.add<ChildrenComponent>(parent, ChildrenComponent{});
    auto& childrenComp = comp.get<ChildrenComponent>(parent);
    childrenComp.children.push_back(entity);
}

void EntityFactory::addRender(Entity entity, int layer, int zIndex)
{
    RenderComponent render{layer, zIndex};
    this->factoryWorld.components().add<RenderComponent>(entity, std::move(render));
}

void EntityFactory::addParallax(Entity entity, const Position& factor)
{
    ParallaxComponent parallax{factor};
    this->factoryWorld.components().add<ParallaxComponent>(entity, std::move(parallax));
}

void EntityFactory::addSprite(Entity entity, const std::string& texturePath)
{
    auto texture = this->resourceManager.load<Texture>(this->textureLoader, texturePath);
    SpriteComponent sprite;
    sprite.texturePath      = texturePath;
    sprite.cachedTexture    = texture;
    sprite.size.width       = static_cast<float>(texture->getWidth());
    sprite.size.height      = static_cast<float>(texture->getHeight());
    sprite.useSourceRect    = false;
    this->factoryWorld.components().add<SpriteComponent>(entity, std::move(sprite));
}

void EntityFactory::addCircleShape(const ShapeParams& params, const Circle& circle)
{
    CircleShapeComponent shape{circle, params.color, params.filled, params.layer};
    this->factoryWorld.components().add<CircleShapeComponent>(params.entity, std::move(shape));
}

void EntityFactory::addRectangleShape(const ShapeParams& params, const Rectangle& rect)
{
    RectangleShapeComponent shape{rect, params.color, params.filled, params.layer};
    this->factoryWorld.components().add<RectangleShapeComponent>(params.entity, std::move(shape));
}

void EntityFactory::addCollider(Entity entity, const Rectangle& rect)
{
    RectangleColliderComponent collider{rect.size};
    this->factoryWorld.components().add<RectangleColliderComponent>(entity, std::move(collider));
}

void EntityFactory::addCollider(Entity entity, const Circle& circle)
{
    CircleColliderComponent collider{circle.radius};
    this->factoryWorld.components().add<CircleColliderComponent>(entity, std::move(collider));
}

void EntityFactory::addDebugVisual(Entity entity, const Rectangle& rect, const DebugConfig& debug)
{
    if (!debug.enabled) return;
    auto& comp = this->factoryWorld.components();
    RectangleShapeComponent shape;
    shape.rect      = rect;
    shape.color     = debug.color;
    shape.filled    = debug.filled;
    shape.layer     = debug.layer;
    shape.zIndex    = debug.zIndex;
    comp.add<RectangleShapeComponent>(entity, std::move(shape));
}

void EntityFactory::addDebugVisual(Entity entity, const Circle& circle, const DebugConfig& debug)
{
    if (!debug.enabled) return;
    auto& comp = this->factoryWorld.components();
    CircleShapeComponent shape;
    shape.circle    = circle;
    shape.color     = debug.color;
    shape.filled    = debug.filled;
    shape.layer     = debug.layer;
    shape.zIndex    = debug.zIndex;
    comp.add<CircleShapeComponent>(entity, std::move(shape));
}
