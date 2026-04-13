#include "../../src/game/include/CharacterDefinitionLoader/CharacterDefinitionLoader.h"

#include "../../src/engine/include/DataNode/DataNode.h"
#include "../../src/engine/include/DataParser/DataParser.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class CharacterDefinitionLoaderFixture
{
public:
    class Node : public DataNode
    {
    public:
        void setString(const std::string& key, const std::string& value) { this->strings[key] = value; }
        void setInt(const std::string& key, int value) { this->ints[key] = value; }

        bool has(const std::string& key) const override
        { return this->strings.contains(key) || this->ints.contains(key); }

        std::string getString(const std::string& key) const override
        {
            const auto it = this->strings.find(key);
            if (it == this->strings.end()) throw std::runtime_error("Missing string key: " + key);
            return it->second;
        }

        int getInt(const std::string& key) const override
        {
            const auto it = this->ints.find(key);
            if (it == this->ints.end()) throw std::runtime_error("Missing int key: " + key);
            return it->second;
        }

        float getFloat(const std::string& key) const override
        {
            (void)key;
            throw std::runtime_error("Float not supported in this fixture");
        }

        bool getBool(const std::string& key) const override
        {
            (void)key;
            throw std::runtime_error("Bool not supported in this fixture");
        }

        std::vector<std::unique_ptr<DataNode>> getArray(const std::string& key) const override
        {
            (void)key;
            throw std::runtime_error("Array not supported in this fixture");
        }

    private:
        std::unordered_map<std::string, std::string> strings;
        std::unordered_map<std::string, int> ints;
    };

    class Parser : public DataParser
    {
    public:
        explicit Parser(std::unique_ptr<DataNode> rootNode) : root(std::move(rootNode)) {}

        mutable std::string lastPath;

        std::unique_ptr<DataNode> parse(const std::string& filePath) const override
        {
            this->lastPath = filePath;

            auto* typed = dynamic_cast<Node*>(root.get());
            if (!typed) throw std::runtime_error("Unexpected root node type in fake parser");

            return std::make_unique<Node>(*typed);
        }

    private:
        std::unique_ptr<DataNode> root;
    };

    std::unique_ptr<DataNode> makeDefinitionRoot(bool withCombos) const
    {
        auto rootNode = std::make_unique<Node>();
        rootNode->setString("id", "fighter_01");
        rootNode->setString("texture", "assets/sprites/fighter.png");
        rootNode->setInt("spriteWidth", 64);
        rootNode->setInt("spriteHeight", 64);
        rootNode->setString("animations", "assets/animations/fighter.json");
        rootNode->setString("stateMachine", "assets/fsm/fighter.json");

        if (withCombos) rootNode->setString("combos", "assets/combos/fighter.json");

        return rootNode;
    }
};

TEST_CASE_METHOD(CharacterDefinitionLoaderFixture, "CharacterDefinitionLoader parses required and optional fields",
    "[unit][character_definition_loader]"
)
{
    Parser parser(this->makeDefinitionRoot(true));
    CharacterDefinitionLoader loader(parser);

    const auto def = loader.load("assets/characters/fighter.json");

    REQUIRE(parser.lastPath == "assets/characters/fighter.json");
    REQUIRE(def.id == "fighter_01");
    REQUIRE(def.texturePath == "assets/sprites/fighter.png");
    REQUIRE(def.spriteWidth == 64);
    REQUIRE(def.spriteHeight == 64);
    REQUIRE(def.animationsPath == "assets/animations/fighter.json");
    REQUIRE(def.stateMachinePath == "assets/fsm/fighter.json");
    REQUIRE(def.combosPath == "assets/combos/fighter.json");
}

TEST_CASE_METHOD(CharacterDefinitionLoaderFixture, "CharacterDefinitionLoader keeps combos path empty when omitted",
    "[unit][character_definition_loader]"
)
{
    Parser parser(this->makeDefinitionRoot(false));
    CharacterDefinitionLoader loader(parser);

    const auto def = loader.load("assets/characters/fighter.json");

    REQUIRE(def.combosPath.empty());
}

TEST_CASE_METHOD(CharacterDefinitionLoaderFixture, "CharacterDefinitionLoader rejects empty id",
    "[unit][character_definition_loader]"
)
{
    auto rootNode = std::make_unique<Node>();
    rootNode->setString("id", "");
    rootNode->setString("texture", "assets/sprites/fighter.png");
    rootNode->setInt("spriteWidth", 32);
    rootNode->setInt("spriteHeight", 32);
    rootNode->setString("animations", "assets/animations/fighter.json");
    rootNode->setString("stateMachine", "assets/fsm/fighter.json");

    Parser parser(std::move(rootNode));
    CharacterDefinitionLoader loader(parser);

    REQUIRE_THROWS_AS(loader.load("assets/characters/fighter.json"), std::runtime_error);
}
