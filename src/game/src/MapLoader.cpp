#include "MapLoader/MapLoader.h"

#include "EntityFactory/EntityFactory.h"

#include "domain/components/ParentComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"
#include "domain/utils/Logger/Logger.h"

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
    this->createFloor(root, mapEntity, world);
    this->createWalls(root, mapEntity);

    return mapEntity;
}

MapComponent MapLoader::parseMapComponent(const std::unique_ptr<DataNode>& root)
{
    MapComponent mapComp;
    mapComp.name = root->getString("name");
    mapComp.gravity = root->getFloat("gravity", 980.f);
    mapComp.floorFriction = root->getFloat("floorFriction", 5.f);
    mapComp.airFriction = root->getFloat("airFriction", 2.f);

    auto floorNode = root->getObject("floor");
    mapComp.floorY = floorNode->getFloat("y");

    if (root->has("worldBounds"))
    {
        auto boundsNode = root->getObject("worldBounds");
        mapComp.worldBounds = {
            boundsNode->getFloat("left"), boundsNode->getFloat("right"),
            boundsNode->getFloat("top"), boundsNode->getFloat("bottom")
        };
    }
    else
    {
        float floorH = floorNode->getFloat("height", 50.f);
        float floorW = floorNode->getFloat("width", 800.f);
        mapComp.worldBounds = { 0.f, floorW, -200.f, mapComp.floorY + floorH + 50.f };
    }

    LOG_DEBUG("MapLoader: worldBounds = [{}, {}, {}, {}]",
        mapComp.worldBounds.left, mapComp.worldBounds.right,
        mapComp.worldBounds.top, mapComp.worldBounds.bottom);

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
    if (root->has("backgroundLayers")) for (auto& layer : root->getArray("backgroundLayers"))
    {
        std::string tex = layer->getString("texture");
        auto parallaxNode = layer->getObject("parallaxFactor");
        Position parallax {
            parallaxNode ? parallaxNode->getFloat("x", 1.f) : 1.f,
            parallaxNode ? parallaxNode->getFloat("y", 1.f) : 1.f
        };
        int zIndex = layer->getInt("zIndex", 0);
        this->factory.createBackgroundChild(tex, parallax, zIndex, mapEntity);
    }
}

void MapLoader::createFloor(const std::unique_ptr<DataNode>& root, Entity mapEntity, World& world)
{
    auto floorNode = root->getObject("floor");
    float floorW = floorNode->getFloat("width");
    float floorH = floorNode->getFloat("height", 50.f);
    std::string floorTex = floorNode->getString("texture", "");
    float floorY = floorNode->getFloat("y");

    Position localPos = { floorW * 0.5f, floorY };
    Dimension2D size = { floorW, floorH };

    LOG_DEBUG("MapLoader: creating floor at local pos ({}, {}) size {}x{}", localPos.x, localPos.y, size.width, size.height);

    Entity floorEntity = this->factory.createFloorChild(floorTex, localPos, size, mapEntity);

    auto& comp = world.components();
    if (comp.has<ParentComponent>(floorEntity))
    {
        Entity parent = comp.get<ParentComponent>(floorEntity).parent;
        LOG_DEBUG("MapLoader: floor entity {} has parent {}", floorEntity.id, parent.id);
    }
    else LOG_WARN("MapLoader: floor entity {} has NO ParentComponent!", floorEntity.id);
}

void MapLoader::createWalls(const std::unique_ptr<DataNode>& root, Entity mapEntity)
{
    if (root->has("walls")) for (auto& wall : root->getArray("walls"))
    {
        float x = wall->getFloat("x");
        float y = wall->getFloat("y");
        float w = wall->getFloat("width");
        float h = wall->getFloat("height");
        this->factory.createWallChild({x + w * 0.5f, y + h * 0.5f}, {w, h}, mapEntity);
    }
}
