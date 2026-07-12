#include "../include/MapLoader/MapLoader.h"
#include "../include/EntityFactory/EntityFactory.h"
#include "../../domain/components/MapComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/RectangleColliderComponent.h"
#include "../../domain/include/World/World.h"

#include <cstdint>

Entity MapLoader::load(World& world, const std::string& path)
{
    auto root = this->parser.parse(path);
    Entity mapEntity = world.entities().create();
    auto& comp = world.components();

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
        mapComp.worldBounds =
        {
            boundsNode->getFloat("left"),
            boundsNode->getFloat("right"),
            boundsNode->getFloat("top"),
            boundsNode->getFloat("bottom")
        };
    }
    else
    {
        float floorH = floorNode->getFloat("height", 50.f);
        float floorW = floorNode->getFloat("width", 800.f);
        mapComp.worldBounds = { 0.f, floorW, -200.f, mapComp.floorY + floorH + 50.f };
    }

    if (root->has("spawnPoints")) for (auto& sp : root->getArray("spawnPoints"))
    {
        MapComponent::SpawnPoint s;
        s.playerId = static_cast<uint32_t>(sp->getInt("playerId"));
        s.x = sp->getFloat("x");
        mapComp.spawnPoints.push_back(s);
    }
    comp.add<MapComponent>(mapEntity, std::move(mapComp));

    comp.add<TransformComponent>(mapEntity, TransformComponent{0.f, 0.f});

    if (root->has("backgroundLayers")) for (auto& layer : root->getArray("backgroundLayers"))
    {
        std::string tex = layer->getString("texture");
        Position parallax
        {
            layer->getFloat("parallaxFactor.x", 1.f),
            layer->getFloat("parallaxFactor.y", 1.f)
        };
        int zIndex = layer->getInt("zIndex", 0);
        this->factory.createBackgroundChild(tex, parallax, zIndex, mapEntity); 
    }

    float floorW = floorNode->getFloat("width");
    float floorH = floorNode->getFloat("height", 50.f);
    std::string floorTex = floorNode->getString("texture", "");
    Entity floorEntity = this->factory.createFloorChild(floorTex, {floorW * 0.5f, mapComp.floorY}, {floorW, floorH}, mapEntity);

    if (root->has("walls")) for (auto& wall : root->getArray("walls"))
    {
        float x = wall->getFloat("x");
        float y = wall->getFloat("y");
        float w = wall->getFloat("width");
        float h = wall->getFloat("height");
        Entity wallEntity = this->factory.createWallChild({ x + w * 0.5f, y + h * 0.5f }, { w, h }, mapEntity); 
    }

    return mapEntity;
}
