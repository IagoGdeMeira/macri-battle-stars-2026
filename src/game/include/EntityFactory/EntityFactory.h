#ifndef entity_factory_h
#define entity_factory_h

#include "AnimationLoader/AnimationLoader.h"

#include "domain/components/PushboxComponent.h"
#include "domain/include/Entity/Entity.h"
#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include <memory>
#include <optional>
#include <string>

class ResourceManager;
class TextureLoader;
class World;

class EntityFactory
{
public:
    struct Config
    {
        World& world;
        ResourceManager& resourceManager;
        TextureLoader& textureLoader;
        AnimationLoader& animationLoader;
    };

    EntityFactory(const Config& cfg) :
        world(cfg.world),
        resourceManager(cfg.resourceManager),
        textureLoader(cfg.textureLoader),
        animLoader(cfg.animationLoader) {}

    ResourceManager& resources() { return this->resourceManager; }
    TextureLoader& texLoader() { return this->textureLoader; }

    struct HitboxChildParams { Entity parent; const Position& offset; int damage; bool facingLeft; };
    Entity createHitboxChild(const HitboxChildParams& params, const Rectangle& rect);
    Entity createHitboxChild(const HitboxChildParams& params, const Circle& circle);

    struct HurtboxChildParams { Entity parent; const Position& offset; float damageMultiplier; bool facingLeft; };
    Entity createHurtboxChild(const HurtboxChildParams& params, const Rectangle& rect);
    Entity createHurtboxChild(const HurtboxChildParams& params, const Circle& circle);

    struct PushboxChildParams { Entity parent; const Position& offset; PushboxComponent::Type type; float mass; float pushResistance; bool facingLeft; };
    Entity createPushboxChild(const PushboxChildParams& params, const Rectangle& rect);
    Entity createPushboxChild(const PushboxChildParams& params, const Circle& circle);

    struct StaticEntityParams { const Position& position; std::optional<Entity> parent; };
    Entity createStaticEntity(const StaticEntityParams& params, const Rectangle& rect);
    Entity createStaticEntity(const StaticEntityParams& params, const Circle& circle);
    
    Entity createEffectSprite(const std::string& texturePath, const Position& position, float duration);
    
    struct BackgroundParams { const Position& parallax; int zIndex; Entity parent; };
    Entity createBackgroundSprite(const BackgroundParams& params, const std::string& texturePath);
    Entity createBackgroundRectangle(const BackgroundParams& params, const Rectangle& rect, const Color& color, bool filled = true);
    Entity createBackgroundCircle(const BackgroundParams& params, const Circle& circle, const Color& color, bool filled = true);
    Entity createBackgroundAnimated(const BackgroundParams& params, const std::string& texturePath, const std::string& animationPath);

private:
    World& world;
    ResourceManager& resourceManager;
    TextureLoader& textureLoader;
    AnimationLoader& animLoader;

    void addParentAndLocal(Entity entity, Entity parent, const Position& localPos);
    void addRender(Entity entity, int layer, int zIndex);
    void addParallax(Entity entity, const Position& factor);
    void addSprite(Entity entity, const std::string& texturePath);

    struct ShapeParams { Entity entity; const Color& color; bool filled; int layer; };
    void addCircleShape(const ShapeParams& params, const Circle& circle);
    void addRectangleShape(const ShapeParams& params, const Rectangle& rect);
    
    void addCollider(Entity entity, const Rectangle& rect);
    void addCollider(Entity entity, const Circle& circle);
};

#endif // entity_factory_h
