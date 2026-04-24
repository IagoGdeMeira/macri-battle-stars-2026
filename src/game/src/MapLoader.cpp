#include "../include/MapLoader/MapLoader.h"

#include <stdexcept>

MapData MapLoader::load(const std::string& path) const
{
    auto root = this->parser.parse(path);
    MapData map;

    map.name = root->getString("name");
    map.floorY = root->getFloat("floor.y");
    map.floorTexture = root->getString("floor.texture");
    map.floorWidth = root->getFloat("floor.width");
    map.floorHeight = root->getFloat("floor.height");
    map.gravity = root->getFloat("gravity", 980.0f);
    map.floorFriction = root->getFloat("floorFriction", 5.0f);
    map.airFriction = root->getFloat("airFriction", 2.0f);

    for (auto& layerNode : root->getArray("backgroundLayers"))
    {
        BackgroundLayer layer;
        layer.texturePath = layerNode->getString("texture");
        layer.parallaxFactorX = layerNode->getFloat("parallaxFactorX");
        layer.parallaxFactorY = layerNode->getFloat("parallaxFactorY");
        layer.zIndex = layerNode->getInt("zIndex");
        layer.repeatX = layerNode->getBool("repeatX");
        map.backgroundLayers.push_back(layer);
    }

    for (auto& wallNode : root->getArray("walls"))
    {
        Rectangle wall;
        wall.position.x = wallNode->getFloat("x");
        wall.position.y = wallNode->getFloat("y");
        wall.width = wallNode->getFloat("width");
        wall.height = wallNode->getFloat("height");
        map.walls.push_back(wall);
    }

    for (auto& spNode : root->getArray("spawnPoints"))
    {
        SpawnPoint sp;
        sp.playerId = spNode->getInt("playerId");
        sp.x = spNode->getFloat("x");
        map.spawnPoints.push_back(sp);
    }

    return map;
}