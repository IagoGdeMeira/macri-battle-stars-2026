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

    mapComp.roundTime = root->getFloat("roundTime", 99.f);

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
        BackgroundParams params{layer, mapEntity}; 
        if (layer->has("parallaxFactor"))
        {
            auto parallaxNode = layer->getObject("parallaxFactor");
            params.parallax = DataUtils::parsePosition(*parallaxNode, {1.f, 1.f});
        }

        if (layer->has("position"))
        {
            auto posNode = layer->getObject("position");
            params.position = DataUtils::parsePosition(*posNode, {0.f, 0.f});
        }

        std::string type = layer->getString("type", "texture");
        if (type == "rectangle") this->createBackgroundRectangle(params);
        else if (type == "circle") this->createBackgroundCircle(params);
        else if (type == "animated") this->createBackgroundAnimated(params);
        else this->createBackgroundTexture(params);
    }
}

void MapLoader::createBackgroundTexture(const BackgroundParams& params)
{
    std::string tex = params.layer->getString("texture");
    int zIndex = params.layer->getInt("zIndex", 0);
    this->factory.createBackgroundSprite(
        EntityFactory::BackgroundParams{params.parallax, zIndex, params.mapEntity, params.position}, tex);
}

void MapLoader::createBackgroundRectangle(const BackgroundParams& params)
{
    Color color = DataUtils::parseColor(*params.layer->getObject("color"), Color::WHITE());
    Rectangle rect = DataUtils::parseRect(*params.layer, {{0.f, 0.f}, {100.f, 100.f}});
    int zIndex = params.layer->getInt("zIndex", -1);
    this->factory.createBackgroundRectangle(EntityFactory::BackgroundParams{
        params.parallax, zIndex, params.mapEntity, params.position}, rect, color, true);
}

void MapLoader::createBackgroundCircle(const BackgroundParams& params)
{
    Color color = DataUtils::parseColor(*params.layer->getObject("color"), Color::WHITE());
    Circle circle = DataUtils::parseCircle(*params.layer, {{0.f, 0.f}, 10.f});
    int zIndex = params.layer->getInt("zIndex", -1);
    this->factory.createBackgroundCircle(EntityFactory::BackgroundParams{
        params.parallax, zIndex, params.mapEntity, params.position}, circle, color, true);
}

void MapLoader::createBackgroundAnimated(const BackgroundParams& params)
{
    std::string tex = params.layer->getString("texture");
    std::string anim = params.layer->getString("animation");
    int zIndex = params.layer->getInt("zIndex", 0);
    this->factory.createBackgroundAnimated(EntityFactory::BackgroundParams{
        params.parallax, zIndex, params.mapEntity, params.position}, tex, anim);
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
