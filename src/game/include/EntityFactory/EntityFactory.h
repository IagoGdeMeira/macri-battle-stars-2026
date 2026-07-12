#ifndef entity_factory_h
#define entity_factory_h

#include "../../domain/include/Entity/Entity.h"
#include "../../domain/value_objects/CollisionFrame/CollisionFrame.h"
#include "../../domain/value_objects/Geometry/Geometry.h"

#include <memory>
#include <string>

class ResourceManager;
class TextureLoader;
class World;

class EntityFactory
{
public:
    EntityFactory(World& world, ResourceManager& resourceManager, TextureLoader& textureLoader) :
        world(world), resourceManager(resourceManager), textureLoader(textureLoader) {}

    Entity createStaticEntity(const Position& position, const Rectangle& rect, Entity parent = Entity::null());
    Entity createStaticEntity(const Position& position, const Circle& circle, Entity parent = Entity::null());
    Entity createPushbox(Entity parent, const PushboxDef& def, bool facingLeft);
    Entity createHitbox(Entity parent, const HitboxDef& def, bool facingLeft);
    Entity createHurtbox(Entity parent, const HurtboxDef& def, bool facingLeft);
    Entity createSpriteEffect(const std::string& texturePath, const Position& position, float duration);
    Entity createBackgroundChild(const std::string& texturePath, const Position& parallax, int zIndex, Entity parent);
    Entity createFloorChild(const std::string& texturePath, const Position& position, const Dimension2D& size, Entity parent);
    Entity createWallChild(const Position& position, const Dimension2D& size, Entity parent);

private:
    World& world;
    ResourceManager& resourceManager;
    TextureLoader& textureLoader;

    Entity createStaticEntityImpl(const Position& position, const ColliderDef& colliderDef, Entity parent);

    void addColliderComponents(Entity entity, const ColliderDef& collider);
    void addDebugVisual(Entity entity, const ColliderDef& collider, const ColliderDebugDef& debug);

    void addSprite(Entity entity, const std::string& texturePath);
    void addRender(Entity entity, int layer, int zIndex);
    void addParallax(Entity entity, const Position& factor);
    void addParentAndLocal(Entity entity, Entity parent, const Position& localPos);
};

#endif // entity_factory_h
