#ifndef entity_factory_h
#define entity_factory_h

#include "../MapData/MapData.h"

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
    EntityFactory(World& world, ResourceManager& resourceManager, TextureLoader& textureLoader);

    Entity createStaticBody(const Position& position);
    Entity addStaticCollider(Entity parent, const Rectangle& rect);
    Entity addStaticCollider(Entity parent, const Circle& circle);
    Entity createBackgroundLayer(const BackgroundLayer& layer);

    Entity createPushbox(Entity parent, const PushboxDef& def, bool facingLeft);
    Entity createHitbox(Entity parent, const HitboxDef& def, bool facingLeft);
    Entity createHurtbox(Entity parent, const HurtboxDef& def, bool facingLeft);

    Entity createSpriteEffect(const std::string& texturePath, const Position& position, float duration);

private:
    World& world;
    ResourceManager& resourceManager;
    TextureLoader& textureLoader;

    void addColliderComponents(Entity entity, const ColliderDef& collider);
    void addDebugVisual(Entity entity, const ColliderDef& collider, const ColliderDebugDef& debug);
};

#endif // entity_factory_h
