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
        { return this->strings.contains(key) || this->ints.contains(key) || this->arrays.contains(key); }

        std::string getString(const std::string& key, const std::string& fallback = DataNode::defaultStringFallback) const override
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
            (void)key;
            return fallback;
        }

        bool getBool(const std::string& key, const bool& fallback = DataNode::defaultBoolFallback) const override
        {
            (void)key;
            return fallback;
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
) {
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
) {
    Parser parser(this->makeDefinitionRoot(false));
    CharacterDefinitionLoader loader(parser);

    const auto def = loader.load("assets/characters/fighter.json");

    REQUIRE(def.combosPath.empty());
}

TEST_CASE_METHOD(CharacterDefinitionLoaderFixture, "CharacterDefinitionLoader rejects empty id",
    "[unit][character_definition_loader]"
) {
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

TEST_CASE_METHOD(CharacterDefinitionLoaderFixture, "CharacterDefinitionLoader parses custom states when present",
    "[unit][character_definition_loader]"
) {
    auto customState = std::make_unique<Node>();
    customState->setString("", "PowerCharge");

    std::vector<std::unique_ptr<DataNode>> customStates;
    customStates.push_back(std::move(customState));

    auto rootNode = this->makeDefinitionRoot(false);
    static_cast<Node*>(rootNode.get())->setArray("customStates", std::move(customStates));

    Parser parser(std::move(rootNode));
    CharacterDefinitionLoader loader(parser);

    const auto def = loader.load("assets/characters/fighter_custom.json");

    REQUIRE(def.customStates.size() == 1);
    REQUIRE(def.customStates[0] == "PowerCharge");
}

TEST_CASE_METHOD(CharacterDefinitionLoaderFixture, "CharacterDefinitionLoader rejects custom states that collide with base states",
    "[unit][character_definition_loader]"
) {
    auto customState = std::make_unique<Node>();
    customState->setString("", "Idle");

    std::vector<std::unique_ptr<DataNode>> customStates;
    customStates.push_back(std::move(customState));

    auto rootNode = this->makeDefinitionRoot(false);
    static_cast<Node*>(rootNode.get())->setArray("customStates", std::move(customStates));

    Parser parser(std::move(rootNode));
    CharacterDefinitionLoader loader(parser);

    REQUIRE_THROWS_AS(loader.load("assets/characters/fighter_invalid.json"), std::runtime_error);
}
