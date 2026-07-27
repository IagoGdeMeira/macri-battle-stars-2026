#include "game/include/EntityFactory/EntityFactory.h"

#include "StubAnimationLoader.h"
#include "StubTextureFactory.h"
#include "StubTextureLoader.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/AnimationComponent.h"
#include "domain/components/AnimationControllerComponent.h"
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

#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/ThreadPool/ThreadPool.h"

#include "game/utils/ComponentRegistry/ComponentRegistry.h"

#include <catch2/catch_test_macros.hpp>

class EntityFactoryFixture
{
public:
    EntityFactoryFixture() :
        threadPool(1),
        textureFactory(),
        textureLoader(this->textureFactory),
        resourceManager(this->threadPool),
        animLoader(),
        factory(EntityFactory::Config{
            this->world, this->resourceManager, this->textureLoader, this->animLoader})
    {
        auto& comp = this->world.components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<RectangleColliderComponent>();
        comp.registerComponent<CircleColliderComponent>();
        comp.registerComponent<PushboxComponent>();
        comp.registerComponent<SpriteComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<ParallaxComponent>();
        comp.registerComponent<LifetimeComponent>();
        comp.registerComponent<HitboxComponent>();
        comp.registerComponent<HurtboxComponent>();
        comp.registerComponent<RectangleShapeComponent>();
        comp.registerComponent<CircleShapeComponent>();
        comp.registerComponent<AnimationControllerComponent>();
        comp.registerComponent<AnimationComponent>();
        comp.registerComponent<ActiveComponent>();
    }

    World world;
    ThreadPool threadPool;
    StubTextureFactory textureFactory;
    StubTextureLoader textureLoader;
    ResourceManager resourceManager;
    StubAnimationLoader animLoader;
    EntityFactory factory;
};

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createStaticEntity() with rectangle creates root entity with TransformComponent",
    "[unit][entity_factory]"
) {
    Position pos{100.f, 200.f};
    Rectangle rect{{0.f, 0.f}, {50.f, 30.f}};

    Entity entity = this->factory.createStaticEntity(EntityFactory::StaticEntityParams{
        pos, std::nullopt}, rect);

    REQUIRE(this->world.entities().isAlive(entity));

    auto& comp = this->world.components();
    REQUIRE(comp.has<TransformComponent>(entity));
    REQUIRE(comp.has<PushboxComponent>(entity));
    REQUIRE(comp.has<RectangleColliderComponent>(entity));
    REQUIRE_FALSE(comp.has<ParentComponent>(entity));
    REQUIRE_FALSE(comp.has<LocalTransform>(entity));

    const auto& transform = comp.get<TransformComponent>(entity);
    REQUIRE(transform.position.x == 100.f);
    REQUIRE(transform.position.y == 200.f);
    REQUIRE(transform.scale.x == 1.f);
    REQUIRE(transform.scale.y == 1.f);
    REQUIRE(transform.rotation == 0.f);

    const auto& collider = comp.get<RectangleColliderComponent>(entity);
    REQUIRE(collider.size.width == 50.f);
    REQUIRE(collider.size.height == 30.f);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createStaticEntity() with circle creates root entity with CircleColliderComponent",
    "[unit][entity_factory]"
) {
    Position pos{100.f, 200.f};
    Circle circle{{0.f, 0.f}, 25.f};

    Entity entity = this->factory.createStaticEntity(EntityFactory::StaticEntityParams{
        pos, std::nullopt}, circle);

    auto& comp = this->world.components();
    REQUIRE(comp.has<TransformComponent>(entity));
    REQUIRE(comp.has<PushboxComponent>(entity));
    REQUIRE(comp.has<CircleColliderComponent>(entity));
    REQUIRE_FALSE(comp.has<RectangleColliderComponent>(entity));

    const auto& collider = comp.get<CircleColliderComponent>(entity);
    REQUIRE(collider.radius == 25.f);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createStaticEntity() with parent creates child entity with ParentComponent and LocalTransform",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    Position localPos{10.f, 20.f};
    Rectangle rect{{0.f, 0.f}, {40.f, 30.f}};

    Entity child = this->factory.createStaticEntity(EntityFactory::StaticEntityParams{
        localPos, parent}, rect);

    auto& comp = this->world.components();
    REQUIRE(comp.has<ParentComponent>(child));
    REQUIRE(comp.has<LocalTransform>(child));
    REQUIRE(comp.has<RectangleColliderComponent>(child));
    REQUIRE(comp.has<PushboxComponent>(child));
    REQUIRE(comp.has<TransformComponent>(child));

    const auto& parentComp = comp.get<ParentComponent>(child);
    REQUIRE(parentComp.parent == parent);

    const auto& local = comp.get<LocalTransform>(child);
    REQUIRE(local.position.x == 10.f);
    REQUIRE(local.position.y == 20.f);
    REQUIRE(local.scale.x == 1.f);
    REQUIRE(local.scale.y == 1.f);
    REQUIRE(local.rotation == 0.f);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createBackgroundSprite() adds all required components",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    Position parallax{0.7f, 0.3f};
    int zIndex = 10;
    std::string texturePath = "assets/sprites/bg.png";

    Entity bg = this->factory.createBackgroundSprite(EntityFactory::BackgroundParams{
        parallax, zIndex, parent}, texturePath);

    auto& comp = this->world.components();
    REQUIRE(comp.has<SpriteComponent>(bg));
    REQUIRE(comp.has<RenderComponent>(bg));
    REQUIRE(comp.has<ParallaxComponent>(bg));
    REQUIRE(comp.has<ParentComponent>(bg));
    REQUIRE(comp.has<LocalTransform>(bg));
    REQUIRE(comp.has<TransformComponent>(bg));
    REQUIRE_FALSE(comp.has<RectangleColliderComponent>(bg));

    const auto& sprite = comp.get<SpriteComponent>(bg);
    REQUIRE(sprite.texturePath == texturePath);
    REQUIRE(sprite.size.width >= 0);
    REQUIRE(sprite.size.height >= 0);
    REQUIRE_FALSE(sprite.useSourceRect);

    const auto& render = comp.get<RenderComponent>(bg);
    REQUIRE(render.layer == 0);
    REQUIRE(render.zIndex == zIndex);

    const auto& p = comp.get<ParallaxComponent>(bg);
    REQUIRE(p.factor.x == 0.7f);
    REQUIRE(p.factor.y == 0.3f);

    const auto& local = comp.get<LocalTransform>(bg);
    REQUIRE(local.position.x == 0.f);
    REQUIRE(local.position.y == 0.f);

    const auto& parentComp = comp.get<ParentComponent>(bg);
    REQUIRE(parentComp.parent == parent);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createBackgroundRectangle() adds RectangleShapeComponent",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    Position parallax{0.5f, 0.5f};
    int zIndex = 5;
    Rectangle rect{{10.f, 20.f}, {100.f, 50.f}};
    Color color{255, 0, 0, 128};

    Entity bgRect = this->factory.createBackgroundRectangle(EntityFactory::BackgroundParams{
        parallax, zIndex, parent}, rect, color, false);

    auto& comp = this->world.components();
    REQUIRE(comp.has<RectangleShapeComponent>(bgRect));
    const auto& shape = comp.get<RectangleShapeComponent>(bgRect);
    REQUIRE(shape.rect.position.x == 10.f);
    REQUIRE(shape.rect.position.y == 20.f);
    REQUIRE(shape.rect.size.width == 100.f);
    REQUIRE(shape.rect.size.height == 50.f);
    REQUIRE(shape.color == color);
    REQUIRE(shape.filled == false);
    REQUIRE(shape.layer == 0);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createBackgroundCircle() adds CircleShapeComponent",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    Position parallax{0.2f, 0.8f};
    int zIndex = 3;
    Circle circle{{0.f, 0.f}, 30.f};
    Color color{0, 255, 0, 200};

    Entity bgCircle = this->factory.createBackgroundCircle(EntityFactory::BackgroundParams{
        parallax, zIndex, parent}, circle, color, true);

    auto& comp = this->world.components();
    REQUIRE(comp.has<CircleShapeComponent>(bgCircle));
    const auto& shape = comp.get<CircleShapeComponent>(bgCircle);
    REQUIRE(shape.circle.position.x == 0.f);
    REQUIRE(shape.circle.position.y == 0.f);
    REQUIRE(shape.circle.radius == 30.f);
    REQUIRE(shape.color == color);
    REQUIRE(shape.filled == true);
    REQUIRE(shape.layer == 0);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createHitboxChild() with rectangle adds HitboxComponent and collider",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    Rectangle rect{{0.f, 0.f}, {20.f, 10.f}};
    Position offset{5.f, 5.f};
    int damage = 10;

    Entity hitbox = this->factory.createHitboxChild(EntityFactory::HitboxChildParams{
        parent, offset, damage, false}, rect);

    auto& comp = this->world.components();
    REQUIRE(comp.has<ParentComponent>(hitbox));
    REQUIRE(comp.has<LocalTransform>(hitbox));
    REQUIRE(comp.has<RectangleColliderComponent>(hitbox));
    REQUIRE(comp.has<HitboxComponent>(hitbox));
    REQUIRE(comp.has<TransformComponent>(hitbox));

    const auto& local = comp.get<LocalTransform>(hitbox);
    REQUIRE(local.position.x == 5.f);
    REQUIRE(local.position.y == 5.f);

    const auto& hit = comp.get<HitboxComponent>(hitbox);
    REQUIRE(hit.damage == 10);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createHitboxChild() with circle and facingLeft inverts offset.x",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    Circle circle{{0.f, 0.f}, 15.f};
    Position offset{-5.f, 10.f};
    int damage = 5;

    Entity hitbox = this->factory.createHitboxChild(EntityFactory::HitboxChildParams{
        parent, offset, damage, true}, circle);

    auto& comp = this->world.components();
    const auto& local = comp.get<LocalTransform>(hitbox);
    REQUIRE(local.position.x == 5.f);
    REQUIRE(local.position.y == 10.f);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createHurtboxChild() adds HurtboxComponent",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    Rectangle rect{{0.f, 0.f}, {30.f, 20.f}};
    Position offset{0.f, 0.f};
    float multiplier = 1.5f;

    Entity hurtbox = this->factory.createHurtboxChild(EntityFactory::HurtboxChildParams{
        parent, offset, multiplier, false}, rect);

    auto& comp = this->world.components();
    REQUIRE(comp.has<HurtboxComponent>(hurtbox));
    const auto& hurt = comp.get<HurtboxComponent>(hurtbox);
    REQUIRE(hurt.damageMultiplier == 1.5f);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createPushboxChild() adds PushboxComponent with correct type and mass",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    Circle circle{{0.f, 0.f}, 25.f};
    Position offset{0.f, 0.f};
    PushboxComponent::Type type = PushboxComponent::Type::Static;
    float mass = 50.f;
    float pushResistance = 0.2f;

    Entity pushbox = this->factory.createPushboxChild(EntityFactory::PushboxChildParams{
        parent, offset, type, mass, pushResistance, false}, circle);

    auto& comp = this->world.components();
    REQUIRE(comp.has<PushboxComponent>(pushbox));
    const auto& push = comp.get<PushboxComponent>(pushbox);
    REQUIRE(push.type == PushboxComponent::Type::Static);
    REQUIRE(push.mass == 50.f);
    REQUIRE(push.pushResistance == 0.2f);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createEffectSprite() adds Sprite, Transform and Lifetime",
    "[unit][entity_factory]"
) {
    Position pos{300.f, 200.f};
    float duration = 2.5f;
    std::string texturePath = "assets/effects/explosion.png";

    Entity effect = this->factory.createEffectSprite(texturePath, pos, duration);

    auto& comp = this->world.components();
    REQUIRE(comp.has<SpriteComponent>(effect));
    REQUIRE(comp.has<TransformComponent>(effect));
    REQUIRE(comp.has<LifetimeComponent>(effect));
    REQUIRE_FALSE(comp.has<ParentComponent>(effect));
    REQUIRE_FALSE(comp.has<LocalTransform>(effect));

    const auto& sprite = comp.get<SpriteComponent>(effect);
    REQUIRE(sprite.texturePath == texturePath);

    const auto& transform = comp.get<TransformComponent>(effect);
    REQUIRE(transform.position.x == 300.f);
    REQUIRE(transform.position.y == 200.f);

    const auto& lifetime = comp.get<LifetimeComponent>(effect);
    REQUIRE(lifetime.remaining == duration);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createBackgroundAnimated() adds Animation components and Sprite",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    Position parallax{0.3f, 0.7f};
    int zIndex = 2;
    std::string texturePath = "assets/sprites/animated.png";
    std::string animationPath = "assets/animations/animated.json";

    Entity animBg = this->factory.createBackgroundAnimated(EntityFactory::BackgroundParams{
        parallax, zIndex, parent}, texturePath, animationPath);

    auto& comp = this->world.components();
    REQUIRE(comp.has<SpriteComponent>(animBg));
    REQUIRE(comp.has<AnimationControllerComponent>(animBg));
    REQUIRE(comp.has<AnimationComponent>(animBg));
    REQUIRE(comp.has<RenderComponent>(animBg));
    REQUIRE(comp.has<ParallaxComponent>(animBg));
    REQUIRE(comp.has<ParentComponent>(animBg));
    REQUIRE(comp.has<LocalTransform>(animBg));
    REQUIRE(comp.has<TransformComponent>(animBg));
}
