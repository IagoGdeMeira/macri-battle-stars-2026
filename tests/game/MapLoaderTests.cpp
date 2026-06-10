#include "../../src/game/include/MapLoader/MapLoader.h"

#include "../stubs/StubDataNode.h"
#include "../stubs/StubDataParser.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <vector>

class MapLoaderFixture
{
public:
    std::unique_ptr<StubDataNode> makeBackgroundLayer() const
    {
        auto layer = std::make_unique<StubDataNode>();
        layer->setString("texture", "assets/maps/bg.png");
        layer->setFloat("parallaxFactorX", 0.5f);
        layer->setFloat("parallaxFactorY", 0.25f);
        layer->setInt("zIndex", 3);
        layer->setBool("repeatX", true);
        return layer;
    }

    std::unique_ptr<StubDataNode> makeWall() const
    {
        auto wall = std::make_unique<StubDataNode>();
        wall->setFloat("x", 10.f);
        wall->setFloat("y", 20.f);
        wall->setFloat("width", 640.f);
        wall->setFloat("height", 48.f);
        return wall;
    }

    std::unique_ptr<StubDataNode> makeSpawnPoint() const
    {
        auto spawn = std::make_unique<StubDataNode>();
        spawn->setInt("playerId", 2);
        spawn->setFloat("x", 320.f);
        return spawn;
    }

    std::unique_ptr<StubDataNode> makeMapRoot() const
    {
        auto root = std::make_unique<StubDataNode>();
        root->setString("name", "Dojo");
        root->setFloat("floor.y", 420.f);
        root->setString("floor.texture", "assets/maps/floor.png");
        root->setFloat("floor.width", 1280.f);
        root->setFloat("floor.height", 64.f);
        root->setFloat("gravity", 900.f);
        root->setFloat("floorFriction", 6.5f);
        root->setFloat("airFriction", 2.75f);

        std::vector<std::unique_ptr<DataNode>> backgrounds;
        backgrounds.push_back(this->makeBackgroundLayer());
        root->setArray("backgroundLayers", std::move(backgrounds));

        std::vector<std::unique_ptr<DataNode>> walls;
        walls.push_back(this->makeWall());
        root->setArray("walls", std::move(walls));

        std::vector<std::unique_ptr<DataNode>> spawnPoints;
        spawnPoints.push_back(this->makeSpawnPoint());
        root->setArray("spawnPoints", std::move(spawnPoints));

        return root;
    }
};

TEST_CASE_METHOD(MapLoaderFixture, "MapLoader parses map data", "[unit][map_loader]")
{
    StubDataParser parser;
    parser.registerNode("assets/maps/dojo.json", this->makeMapRoot());

    MapLoader loader(parser);
    const auto map = loader.load("assets/maps/dojo.json");

    REQUIRE(map.name == "Dojo");
    REQUIRE(map.floorY == 420.f);
    REQUIRE(map.floorTexture == "assets/maps/floor.png");
    REQUIRE(map.floorWidth == 1280.f);
    REQUIRE(map.floorHeight == 64.f);
    REQUIRE(map.gravity == 900.f);
    REQUIRE(map.floorFriction == 6.5f);
    REQUIRE(map.airFriction == 2.75f);

    REQUIRE(map.backgroundLayers.size() == 1);
    REQUIRE(map.backgroundLayers[0].texturePath == "assets/maps/bg.png");
    REQUIRE(map.backgroundLayers[0].parallaxFactorX == 0.5f);
    REQUIRE(map.backgroundLayers[0].parallaxFactorY == 0.25f);
    REQUIRE(map.backgroundLayers[0].zIndex == 3);
    REQUIRE(map.backgroundLayers[0].repeatX == true);

    REQUIRE(map.walls.size() == 1);
    REQUIRE(map.walls[0].position.x == 10.f);
    REQUIRE(map.walls[0].position.y == 20.f);
    REQUIRE(map.walls[0].size.width == 640.f);
    REQUIRE(map.walls[0].size.height == 48.f);

    REQUIRE(map.spawnPoints.size() == 1);
    REQUIRE(map.spawnPoints[0].playerId == 2u);
    REQUIRE(map.spawnPoints[0].x == 320.f);
}
