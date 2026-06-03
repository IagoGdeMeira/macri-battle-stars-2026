#include "../../src/game/include/MapLoader/MapLoader.h"

#include "../../src/engine/include/DataNode/DataNode.h"
#include "../../src/engine/include/DataParser/DataParser.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class MapLoaderFixture
{
public:
    class Node : public DataNode
    {
    public:
        void setString(const std::string& key, const std::string& value) { this->strings[key] = value; }
        void setInt(const std::string& key, int value) { this->ints[key] = value; }
        void setFloat(const std::string& key, float value) { this->floats[key] = value; }
        void setBool(const std::string& key, bool value) { this->bools[key] = value; }

        void setArray(const std::string& key, std::vector<std::unique_ptr<DataNode>> value)
        {
            std::vector<Node> converted;
            converted.reserve(value.size());

            for (auto& node : value)
            {
                auto* typed = dynamic_cast<Node*>(node.get());
                if (!typed) throw std::runtime_error("Unexpected node type in fake tree");
                converted.push_back(*typed);
            }

            this->arrays[key] = std::move(converted);
        }

        bool has(const std::string& key) const override
        {
            return this->strings.find(key) != this->strings.end()
                || this->ints.find(key) != this->ints.end()
                || this->floats.find(key) != this->floats.end()
                || this->bools.find(key) != this->bools.end()
                || this->arrays.find(key) != this->arrays.end();
        }

        std::string getString(const std::string& key,
            const std::string& fallback = DataNode::defaultStringFallback) const override
        {
            const auto it = this->strings.find(key);
            if (it == this->strings.end()) return fallback;
            return it->second;
        }

        int getInt(const std::string& key, const int& fallback = DataNode::defaultIntFallback) const override
        {
            const auto it = this->ints.find(key);
            if (it == this->ints.end()) return fallback;
            return it->second;
        }

        float getFloat(const std::string& key, const float& fallback = DataNode::defaultFloatFallback) const override
        {
            const auto it = this->floats.find(key);
            if (it == this->floats.end()) return fallback;
            return it->second;
        }

        bool getBool(const std::string& key, const bool& fallback = DataNode::defaultBoolFallback) const override
        {
            const auto it = this->bools.find(key);
            if (it == this->bools.end()) return fallback;
            return it->second;
        }

        std::vector<std::unique_ptr<DataNode>> getArray(const std::string& key) const override
        {
            const auto it = this->arrays.find(key);
            if (it == this->arrays.end()) throw std::runtime_error("Missing array key: " + key);

            std::vector<std::unique_ptr<DataNode>> out;
            out.reserve(it->second.size());

            for (const auto& node : it->second)
            { out.push_back(std::make_unique<Node>(node)); }

            return out;
        }

    private:
        std::unordered_map<std::string, std::string> strings;
        std::unordered_map<std::string, int> ints;
        std::unordered_map<std::string, float> floats;
        std::unordered_map<std::string, bool> bools;
        std::unordered_map<std::string, std::vector<Node>> arrays;
    };

    class Parser : public DataParser
    {
    public:
        explicit Parser(std::unique_ptr<DataNode> rootNode) : root(std::move(rootNode)) {}

        mutable std::string lastPath;

        std::unique_ptr<DataNode> parse(const std::string& filePath) const override
        {
            this->lastPath = filePath;

            auto* typed = dynamic_cast<Node*>(this->root.get());
            if (!typed) throw std::runtime_error("Unexpected root node type in fake parser");

            return std::make_unique<Node>(*typed);
        }

    private:
        std::unique_ptr<DataNode> root;
    };

    static std::unique_ptr<DataNode> makeBackgroundLayer()
    {
        auto layer = std::make_unique<Node>();
        layer->setString("texture", "assets/maps/bg.png");
        layer->setFloat("parallaxFactorX", 0.5f);
        layer->setFloat("parallaxFactorY", 0.25f);
        layer->setInt("zIndex", 3);
        layer->setBool("repeatX", true);
        return layer;
    }

    static std::unique_ptr<DataNode> makeWall()
    {
        auto wall = std::make_unique<Node>();
        wall->setFloat("x", 10.f);
        wall->setFloat("y", 20.f);
        wall->setFloat("width", 640.f);
        wall->setFloat("height", 48.f);
        return wall;
    }

    static std::unique_ptr<DataNode> makeSpawnPoint()
    {
        auto spawn = std::make_unique<Node>();
        spawn->setInt("playerId", 2);
        spawn->setFloat("x", 320.f);
        return spawn;
    }

    static std::unique_ptr<DataNode> makeMapRoot()
    {
        auto root = std::make_unique<Node>();
        root->setString("name", "Dojo");
        root->setFloat("floor.y", 420.f);
        root->setString("floor.texture", "assets/maps/floor.png");
        root->setFloat("floor.width", 1280.f);
        root->setFloat("floor.height", 64.f);
        root->setFloat("gravity", 900.f);
        root->setFloat("floorFriction", 6.5f);
        root->setFloat("airFriction", 2.75f);

        std::vector<std::unique_ptr<DataNode>> backgrounds;
        backgrounds.push_back(makeBackgroundLayer());
        root->setArray("backgroundLayers", std::move(backgrounds));

        std::vector<std::unique_ptr<DataNode>> walls;
        walls.push_back(makeWall());
        root->setArray("walls", std::move(walls));

        std::vector<std::unique_ptr<DataNode>> spawnPoints;
        spawnPoints.push_back(makeSpawnPoint());
        root->setArray("spawnPoints", std::move(spawnPoints));

        return root;
    }
};

TEST_CASE_METHOD(MapLoaderFixture, "MapLoader parses map data", "[unit][map_loader]")
{
    Parser parser(makeMapRoot());
    MapLoader loader(parser);

    const auto map = loader.load("assets/maps/dojo.json");

    REQUIRE(parser.lastPath == "assets/maps/dojo.json");
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
