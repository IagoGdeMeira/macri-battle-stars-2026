#include "MapLoader/MapLoader.h"

#include "EntityFactory/EntityFactory.h"
#include "PushboxLoader/PushboxLoader.h"

#include "domain/components/ParentComponent.h"
#include "domain/components/PushboxComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/TextureLoader/TextureLoader.h"
#include "engine/utils/DataUtils/DataUtils.h"

#include <cstdint>

Entity MapLoader::load(World& world, const std::string& path)
{
    auto root = this->parser.parse(path);
    Entity mapEntity = world.entities().create();
    auto& comp = world.components();

    MapComponent mapComp = this->parseMapComponent(root);
    comp.add<MapComponent>(mapEntity, std::move(mapComp));
    comp.add<TransformComponent>(mapEntity, TransformComponent{0.f, 0.f});

    this->createBackgrounds(root, mapEntity);
    this->createCollisionGeometry(root, mapEntity);

    return mapEntity;
}

MapComponent MapLoader::parseMapComponent(const std::unique_ptr<DataNode>& root)
{
    MapComponent mapComp;
    mapComp.name = root->getString("name");
    mapComp.gravity = root->getFloat("gravity", 980.f);
    mapComp.floorFriction = root->getFloat("floorFriction", 5.f);
    mapComp.airFriction = root->getFloat("airFriction", 2.f);

    if (root->has("floor"))
    {
        auto floorNode = root->getObject("floor");
        mapComp.floorY = floorNode->getFloat("y");
    }
    else mapComp.floorY = 400.f;

    if (root->has("worldBounds"))
    {
        auto boundsNode = root->getObject("worldBounds");
        mapComp.worldBounds = DataUtils::parseAABB(*boundsNode, {0.f, 2000.f, -200.f, 500.f});
    }
    else
    {
        float floorW = root->has("floor") ? root->getObject("floor")->getFloat("width", 800.f) : 800.f;
        mapComp.worldBounds = { 0.f, floorW, -200.f, mapComp.floorY + 50.f + 50.f };
    }

    if (root->has("spawnPoints")) for (auto& sp : root->getArray("spawnPoints"))
    {
        MapComponent::SpawnPoint s;
        s.playerId = static_cast<uint32_t>(sp->getInt("playerId"));
        s.x = sp->getFloat("x");
        mapComp.spawnPoints.push_back(s);
    }

    return mapComp;
}

void MapLoader::createBackgrounds(const std::unique_ptr<DataNode>& root, Entity mapEntity)
{
    if (!root->has("backgroundLayers")) return;

    for (auto& layer : root->getArray("backgroundLayers"))
    {
        std::string type = layer->getString("type", "texture");
        if (type == "rectangle")        this->createBackgroundRectangle(layer, mapEntity);
        else if (type == "circle")      this->createBackgroundCircle(layer, mapEntity);
        else if (type == "animated")    this->createBackgroundAnimated(layer, mapEntity);
        else this->createBackgroundTexture(layer, mapEntity);
    }
}

void MapLoader::createBackgroundTexture(const std::unique_ptr<DataNode>& layer, Entity mapEntity)
{
    std::string tex = layer->getString("texture");
    auto parallaxNode = layer->getObject("parallaxFactor");
    Position parallax {
        parallaxNode ? parallaxNode->getFloat("x", 1.f) : 1.f,
        parallaxNode ? parallaxNode->getFloat("y", 1.f) : 1.f
    };
    int zIndex = layer->getInt("zIndex", 0);

    this->factory.createBackgroundSprite(EntityFactory::BackgroundParams{parallax, zIndex, mapEntity}, tex);
}

void MapLoader::createBackgroundRectangle(const std::unique_ptr<DataNode>& layer, Entity mapEntity)
{
    Color color = DataUtils::parseColor(*layer->getObject("color"), Color::WHITE());
    Rectangle rect = DataUtils::parseRect(*layer, {{0.f, 0.f}, {100.f, 100.f}});
    int zIndex = layer->getInt("zIndex", -1);

    this->factory.createBackgroundRectangle(EntityFactory::BackgroundParams{
        {1.f, 1.f}, zIndex, mapEntity}, rect, color, true);
}

void MapLoader::createBackgroundCircle(const std::unique_ptr<DataNode>& layer, Entity mapEntity)
{
    Color color = DataUtils::parseColor(*layer->getObject("color"), Color::WHITE());
    Circle circle = DataUtils::parseCircle(*layer, {{0.f, 0.f}, 10.f});
    int zIndex = layer->getInt("zIndex", -1);

    this->factory.createBackgroundCircle(EntityFactory::BackgroundParams{
        {1.f, 1.f}, zIndex, mapEntity}, circle, color, true);
}

void MapLoader::createBackgroundAnimated(const std::unique_ptr<DataNode>& layer, Entity mapEntity)
{
    std::string tex = layer->getString("texture");
    std::string anim = layer->getString("animation");
    auto parallaxNode = layer->getObject("parallaxFactor");
    Position parallax {
        parallaxNode ? parallaxNode->getFloat("x", 1.f) : 1.f,
        parallaxNode ? parallaxNode->getFloat("y", 1.f) : 1.f
    };
    int zIndex = layer->getInt("zIndex", 0);

    this->factory.createBackgroundAnimated(EntityFactory::BackgroundParams{
        parallax, zIndex, mapEntity}, tex, anim);
}

void MapLoader::createCollisionGeometry(const std::unique_ptr<DataNode>& root, Entity mapEntity)
{
    if (!root->has("collisions")) return;

    for (auto& collNode : root->getArray("collisions"))
    {
        if (!collNode->has("pushboxType")) const_cast<DataNode*>(collNode.get())->setString("pushboxType", "static");
        this->pushboxLoader.createStaticPushbox(*collNode, mapEntity);
    }
}
