#include "../../src/game/include/EntityFactory/EntityFactory.h"

#include "../stubs/StubTextureFactory.h"
#include "../stubs/StubTextureLoader.h"

#include "../../src/domain/include/World/World.h"
#include "../../src/domain/components/TransformComponent.h"
#include "../../src/domain/components/ParentComponent.h"
#include "../../src/domain/components/LocalTransform.h"
#include "../../src/domain/components/SpriteComponent.h"
#include "../../src/domain/components/RenderComponent.h"
#include "../../src/domain/components/ParallaxComponent.h"
#include "../../src/domain/components/RectangleColliderComponent.h"
#include "../../src/domain/components/CircleColliderComponent.h"
#include "../../src/domain/components/PushboxComponent.h"
#include "../../src/domain/components/HitboxComponent.h"
#include "../../src/domain/components/HurtboxComponent.h"
#include "../../src/domain/components/LifetimeComponent.h"
#include "../../src/domain/components/ShapeRenderComponent.h"

#include "../../src/engine/include/ResourceManager/ResourceManager.h"
#include "../../src/engine/include/ThreadPool/ThreadPool.h"

#include "../../src/game/include/ComponentRegistry/ComponentRegistry.h"

#include <catch2/catch_test_macros.hpp>

class EntityFactoryFixture
{
public:
    EntityFactoryFixture() :
        threadPool(1),
        textureFactory(),
        textureLoader(this->textureFactory),
        resourceManager(this->threadPool),
        factory(this->world, this->resourceManager, this->textureLoader)
    { ComponentRegistry::registerAll(this->world.components()); }

    World world;
    ThreadPool threadPool;
    StubTextureFactory textureFactory;
    StubTextureLoader textureLoader;
    ResourceManager resourceManager;
    EntityFactory factory;
};

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createStaticEntity() with rectangle creates root entity with TransformComponent",
    "[unit][entity_factory]"
) {
    Position pos{100.f, 200.f};
    Rectangle rect{{0.f, 0.f}, {50.f, 30.f}};

    Entity entity = this->factory.createStaticEntity(pos, rect);

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

TEST_CASE_METHOD(EntityFactoryFixture,
    "EntityFactory::createStaticEntity() with circle creates root entity with CircleColliderComponent",
    "[unit][entity_factory]"
) {
    Position pos{100.f, 200.f};
    Circle circle{{0.f, 0.f}, 25.f};

    Entity entity = this->factory.createStaticEntity(pos, circle);

    auto& comp = this->world.components();
    REQUIRE(comp.has<TransformComponent>(entity));
    REQUIRE(comp.has<PushboxComponent>(entity));
    REQUIRE(comp.has<CircleColliderComponent>(entity));
    REQUIRE_FALSE(comp.has<RectangleColliderComponent>(entity));

    const auto& collider = comp.get<CircleColliderComponent>(entity);
    REQUIRE(collider.radius == 25.f);
}

TEST_CASE_METHOD(EntityFactoryFixture,
    "EntityFactory::createStaticEntity() with parent creates child entity with ParentComponent and LocalTransform",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    Position localPos{10.f, 20.f};
    Rectangle rect{{0.f, 0.f}, {40.f, 30.f}};

    Entity child = this->factory.createStaticEntity(localPos, rect, parent);

    auto& comp = this->world.components();
    REQUIRE(comp.has<ParentComponent>(child));
    REQUIRE(comp.has<LocalTransform>(child));
    REQUIRE(comp.has<RectangleColliderComponent>(child));
    REQUIRE(comp.has<PushboxComponent>(child));
    REQUIRE_FALSE(comp.has<TransformComponent>(child));

    const auto& parentComp = comp.get<ParentComponent>(child);
    REQUIRE(parentComp.parent == parent);

    const auto& local = comp.get<LocalTransform>(child);
    REQUIRE(local.position.x == 10.f);
    REQUIRE(local.position.y == 20.f);
    REQUIRE(local.scale.x == 1.f);
    REQUIRE(local.scale.y == 1.f);
    REQUIRE(local.rotation == 0.f);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createBackgroundChild() adds all required components",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    Position parallax{0.7f, 0.3f};
    int zIndex = 10;

    Entity bg = this->factory.createBackgroundChild("assets/sprites/bg.png", parallax, zIndex, parent);

    auto& comp = this->world.components();
    REQUIRE(comp.has<SpriteComponent>(bg));
    REQUIRE(comp.has<RenderComponent>(bg));
    REQUIRE(comp.has<ParallaxComponent>(bg));
    REQUIRE(comp.has<ParentComponent>(bg));
    REQUIRE(comp.has<LocalTransform>(bg));
    REQUIRE_FALSE(comp.has<TransformComponent>(bg));
    REQUIRE_FALSE(comp.has<RectangleColliderComponent>(bg));

    const auto& sprite = comp.get<SpriteComponent>(bg);
    REQUIRE(sprite.texture != nullptr);
    REQUIRE(sprite.size.width == 64.f);
    REQUIRE(sprite.size.height == 96.f);
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

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createFloorChild() with texture adds Sprite and RenderComponent",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    Position pos{150.f, 400.f};
    Dimension2D size{800.f, 50.f};

    Entity floor = this->factory.createFloorChild("assets/sprites/floor.png", pos, size, parent);

    auto& comp = this->world.components();
    REQUIRE(comp.has<RectangleColliderComponent>(floor));
    REQUIRE(comp.has<ParentComponent>(floor));
    REQUIRE(comp.has<LocalTransform>(floor));
    REQUIRE(comp.has<SpriteComponent>(floor));
    REQUIRE(comp.has<RenderComponent>(floor));
    REQUIRE_FALSE(comp.has<TransformComponent>(floor));

    const auto& collider = comp.get<RectangleColliderComponent>(floor);
    REQUIRE(collider.size.width == 800.f);
    REQUIRE(collider.size.height == 50.f);

    const auto& local = comp.get<LocalTransform>(floor);
    REQUIRE(local.position.x == 150.f);
    REQUIRE(local.position.y == 400.f);

    const auto& sprite = comp.get<SpriteComponent>(floor);
    REQUIRE(sprite.texture != nullptr);
    REQUIRE(sprite.size.width == 800.f);
    REQUIRE(sprite.size.height == 50.f);
    REQUIRE_FALSE(sprite.useSourceRect);
}

TEST_CASE_METHOD(EntityFactoryFixture,
    "EntityFactory::createFloorChild() without texture does not add Sprite or RenderComponent",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    Position pos{150.f, 400.f};
    Dimension2D size{800.f, 50.f};

    Entity floor = this->factory.createFloorChild("", pos, size, parent);

    auto& comp = this->world.components();
    REQUIRE(comp.has<RectangleColliderComponent>(floor));
    REQUIRE(comp.has<ParentComponent>(floor));
    REQUIRE(comp.has<LocalTransform>(floor));
    REQUIRE_FALSE(comp.has<SpriteComponent>(floor));
    REQUIRE_FALSE(comp.has<RenderComponent>(floor));
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createWallChild() adds RectangleCollider, Parent and LocalTransform",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    Position pos{200.f, 300.f};
    Dimension2D size{40.f, 120.f};

    Entity wall = this->factory.createWallChild(pos, size, parent);

    auto& comp = this->world.components();
    REQUIRE(comp.has<RectangleColliderComponent>(wall));
    REQUIRE(comp.has<ParentComponent>(wall));
    REQUIRE(comp.has<LocalTransform>(wall));
    REQUIRE_FALSE(comp.has<TransformComponent>(wall));
    REQUIRE_FALSE(comp.has<SpriteComponent>(wall));
    REQUIRE_FALSE(comp.has<RenderComponent>(wall));

    const auto& collider = comp.get<RectangleColliderComponent>(wall);
    REQUIRE(collider.size.width == 40.f);
    REQUIRE(collider.size.height == 120.f);

    const auto& local = comp.get<LocalTransform>(wall);
    REQUIRE(local.position.x == 200.f);
    REQUIRE(local.position.y == 300.f);
}

TEST_CASE_METHOD(EntityFactoryFixture,
    "EntityFactory::createHitbox() facingRight adds LocalTransform with positive offset",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    HitboxDef def;
    def.collider = std::make_unique<RectangleDef>();
    def.collider->offset = {20.f, 10.f};
    static_cast<RectangleDef*>(def.collider.get())->width = 30.f;
    static_cast<RectangleDef*>(def.collider.get())->height = 20.f;
    def.damage = 10;

    Entity hitbox = this->factory.createHitbox(parent, def, false);

    auto& comp = this->world.components();
    REQUIRE(comp.has<ParentComponent>(hitbox));
    REQUIRE(comp.has<LocalTransform>(hitbox));
    REQUIRE(comp.has<RectangleColliderComponent>(hitbox));
    REQUIRE(comp.has<HitboxComponent>(hitbox));
    REQUIRE_FALSE(comp.has<TransformComponent>(hitbox));

    const auto& local = comp.get<LocalTransform>(hitbox);
    REQUIRE(local.position.x == 20.f);
    REQUIRE(local.position.y == 10.f);

    const auto& hit = comp.get<HitboxComponent>(hitbox);
    REQUIRE(hit.damage == 10);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createHitbox() facingLeft inverts offset.x",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    HitboxDef def;
    def.collider = std::make_unique<RectangleDef>();
    def.collider->offset = {20.f, 10.f};
    static_cast<RectangleDef*>(def.collider.get())->width = 30.f;
    static_cast<RectangleDef*>(def.collider.get())->height = 20.f;
    def.damage = 15;

    Entity hitbox = this->factory.createHitbox(parent, def, true);

    auto& comp = this->world.components();
    const auto& local = comp.get<LocalTransform>(hitbox);
    REQUIRE(local.position.x == -20.f);
    REQUIRE(local.position.y == 10.f);

    const auto& hit = comp.get<HitboxComponent>(hitbox);
    REQUIRE(hit.damage == 15);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createHurtbox() adds HurtboxComponent with damageMultiplier",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    HurtboxDef def;
    def.collider = std::make_unique<CircleDef>();
    def.collider->offset = {5.f, 5.f};
    static_cast<CircleDef*>(def.collider.get())->radius = 15.f;
    def.damageMultiplier = 2.5f;

    Entity hurtbox = this->factory.createHurtbox(parent, def, false);

    auto& comp = this->world.components();
    REQUIRE(comp.has<ParentComponent>(hurtbox));
    REQUIRE(comp.has<LocalTransform>(hurtbox));
    REQUIRE(comp.has<CircleColliderComponent>(hurtbox));
    REQUIRE(comp.has<HurtboxComponent>(hurtbox));

    const auto& local = comp.get<LocalTransform>(hurtbox);
    REQUIRE(local.position.x == 5.f);
    REQUIRE(local.position.y == 5.f);

    const auto& hurt = comp.get<HurtboxComponent>(hurtbox);
    REQUIRE(hurt.damageMultiplier == 2.5f);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createPushbox() adds PushboxComponent with type and mass",
    "[unit][entity_factory]"
) {
    Entity parent = this->world.entities().create();
    PushboxDef def;
    def.collider = std::make_unique<RectangleDef>();
    def.collider->offset = {0.f, 0.f};
    static_cast<RectangleDef*>(def.collider.get())->width = 40.f;
    static_cast<RectangleDef*>(def.collider.get())->height = 30.f;
    def.type = PushboxComponent::PushboxType::Static;
    def.mass = 100.f;
    def.pushResistance = 0.8f;

    Entity pushbox = this->factory.createPushbox(parent, def, false);

    auto& comp = this->world.components();
    REQUIRE(comp.has<ParentComponent>(pushbox));
    REQUIRE(comp.has<LocalTransform>(pushbox));
    REQUIRE(comp.has<RectangleColliderComponent>(pushbox));
    REQUIRE(comp.has<PushboxComponent>(pushbox));

    const auto& push = comp.get<PushboxComponent>(pushbox);
    REQUIRE(push.type == PushboxComponent::PushboxType::Static);
    REQUIRE(push.mass == 100.f);
    REQUIRE(push.pushResistance == 0.8f);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createSpriteEffect() adds Sprite, Transform and Lifetime",
    "[unit][entity_factory]"
) {
    Position pos{300.f, 200.f};
    float duration = 2.5f;

    Entity effect = this->factory.createSpriteEffect("assets/effects/explosion.png", pos, duration);

    auto& comp = this->world.components();
    REQUIRE(comp.has<SpriteComponent>(effect));
    REQUIRE(comp.has<TransformComponent>(effect));
    REQUIRE(comp.has<LifetimeComponent>(effect));
    REQUIRE_FALSE(comp.has<ParentComponent>(effect));
    REQUIRE_FALSE(comp.has<LocalTransform>(effect));

    const auto& sprite = comp.get<SpriteComponent>(effect);
    REQUIRE(sprite.texture != nullptr);
    REQUIRE(sprite.size.width == 64.f);
    REQUIRE(sprite.size.height == 96.f);

    const auto& transform = comp.get<TransformComponent>(effect);
    REQUIRE(transform.position.x == 300.f);
    REQUIRE(transform.position.y == 200.f);

    const auto& lifetime = comp.get<LifetimeComponent>(effect);
    REQUIRE(lifetime.remaining == duration);
}

TEST_CASE_METHOD(EntityFactoryFixture, "EntityFactory::createStaticEntity() adds ShapeRenderComponent if debug enabled",
    "[unit][entity_factory]"
) {
    Position pos{50.f, 50.f};
    Rectangle rect{{0.f, 0.f}, {20.f, 20.f}};

    Entity entity = this->factory.createStaticEntity(pos, rect);

    auto& comp = this->world.components();
    REQUIRE(comp.has<ShapeRenderComponent>(entity));

    const auto& shape = comp.get<ShapeRenderComponent>(entity);
    REQUIRE(shape.filled == false);
    REQUIRE(shape.layer == 10);
    REQUIRE(shape.shape != nullptr);
}
