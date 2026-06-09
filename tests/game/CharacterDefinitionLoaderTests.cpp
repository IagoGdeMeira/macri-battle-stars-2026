#include "../../src/game/include/CharacterDefinitionLoader/CharacterDefinitionLoader.h"

#include "../stubs/StubDataNode.h"
#include "../stubs/StubDataParser.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class CharacterDefinitionLoaderFixture
{
public:
    StubDataParser parser;

    CharacterDefinitionLoaderFixture()
    {
        this->parser.registerNode("assets/characters/fighter.json", this->makeDefinitionRoot(true, true));
        this->parser.registerNode("assets/characters/fighter_no_opt.json", this->makeDefinitionRoot(false, false));
    }

    std::unique_ptr<StubDataNode> makeSizeNode(float w, float h) const
    {
        auto sizeNode = std::make_unique<StubDataNode>();
        sizeNode->setFloat("w", w);
        sizeNode->setFloat("h", h);
        return sizeNode;
    }

    std::unique_ptr<StubDataNode> makeDefinitionRoot(bool withCombos, bool withCollisions = false) const
    {
        auto rootNode = std::make_unique<StubDataNode>();
        rootNode->setString("id", "fighter_01");
        rootNode->setString("texture", "assets/sprites/fighter.png");
        rootNode->setObject("spriteSize", this->makeSizeNode(64.f, 96.f));
        rootNode->setString("animations", "assets/animations/fighter.json");
        rootNode->setString("stateMachine", "assets/fsm/fighter.json");

        if (withCombos) rootNode->setString("combos", "assets/combos/fighter.json");
        if (withCollisions) rootNode->setString("collisions", "assets/collisions/fighter.json");

        return rootNode;
    }
};

TEST_CASE_METHOD(CharacterDefinitionLoaderFixture,
    "CharacterDefinitionLoader parses required and optional fields",
    "[unit][character_definition_loader]"
) {
    CharacterDefinitionLoader loader(this->parser);
    const auto def = loader.load("assets/characters/fighter.json");

    REQUIRE(def.id == "fighter_01");
    REQUIRE(def.texturePath == "assets/sprites/fighter.png");
    REQUIRE(def.spriteSize.width == 64.f);
    REQUIRE(def.spriteSize.height == 96.f);
    REQUIRE(def.animationsPath == "assets/animations/fighter.json");
    REQUIRE(def.stateMachinePath == "assets/fsm/fighter.json");
    REQUIRE(def.combosPath == "assets/combos/fighter.json");
    REQUIRE(def.collisionsPath == "assets/collisions/fighter.json");
}

TEST_CASE_METHOD(CharacterDefinitionLoaderFixture, "CharacterDefinitionLoader handles missing size", 
    "[unit][character_definition_loader]"
) {
    auto rootNode = std::make_unique<StubDataNode>();
    rootNode->setString("id", "fighter_01");
    rootNode->setString("texture", "assets/sprites/fighter.png");
    rootNode->setString("animations", "assets/animations/fighter.json");
    rootNode->setString("stateMachine", "assets/fsm/fighter.json");
    StubDataParser localParser;
    localParser.registerNode("test.json", std::move(rootNode));
    CharacterDefinitionLoader loader(localParser);
    const auto def = loader.load("test.json");
    REQUIRE(def.spriteSize.width == 0.f);
    REQUIRE(def.spriteSize.height == 0.f);
}

TEST_CASE_METHOD(CharacterDefinitionLoaderFixture, "CharacterDefinitionLoader rejects empty id",
    "[unit][character_definition_loader]"
) {
    auto rootNode = std::make_unique<StubDataNode>();
    rootNode->setString("id", "");
    rootNode->setString("texture", "assets/sprites/fighter.png");
    rootNode->setObject("spriteSize", this->makeSizeNode(64.f, 96.f));
    rootNode->setString("animations", "assets/animations/fighter.json");
    rootNode->setString("stateMachine", "assets/fsm/fighter.json");

    StubDataParser localParser;
    localParser.registerNode("test.json", std::move(rootNode));
    CharacterDefinitionLoader loader(localParser);

    REQUIRE_THROWS_AS(loader.load("test.json"), std::runtime_error);
}

TEST_CASE_METHOD(CharacterDefinitionLoaderFixture,
    "CharacterDefinitionLoader parses custom states when present",
    "[unit][character_definition_loader]"
) {
    auto customState = std::make_unique<StubDataNode>();
    customState->setString("", "PowerCharge");

    std::vector<std::unique_ptr<DataNode>> customStates;
    customStates.push_back(std::move(customState));

    auto rootNode = this->makeDefinitionRoot(false);
    rootNode->setArray("customStates", std::move(customStates));

    StubDataParser localParser;
    localParser.registerNode("test.json", std::move(rootNode));
    CharacterDefinitionLoader loader(localParser);

    const auto def = loader.load("test.json");

    REQUIRE(def.customStates.size() == 1);
    REQUIRE(def.customStates[0] == "PowerCharge");
}

TEST_CASE_METHOD(CharacterDefinitionLoaderFixture,
    "CharacterDefinitionLoader rejects custom states that collide with base states",
    "[unit][character_definition_loader]"
) {
    auto customState = std::make_unique<StubDataNode>();
    customState->setString("", "Idle");

    std::vector<std::unique_ptr<DataNode>> customStates;
    customStates.push_back(std::move(customState));

    auto rootNode = this->makeDefinitionRoot(false);
    rootNode->setArray("customStates", std::move(customStates));

    StubDataParser localParser;
    localParser.registerNode("test.json", std::move(rootNode));
    CharacterDefinitionLoader loader(localParser);

    REQUIRE_THROWS_AS(loader.load("test.json"), std::runtime_error);
}
