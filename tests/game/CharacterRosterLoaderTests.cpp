#include "../../src/game/include/CharacterRosterLoader/CharacterRosterLoader.h"

#include "../stubs/StubDataNode.h"
#include "../stubs/StubDataParser.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <vector>

class CharacterRosterLoaderFixture
{
public:
    std::unique_ptr<StubDataNode> makeCharacter(const std::string& id, const std::string& name, const std::string& definition) const
    {
        auto node = std::make_unique<StubDataNode>();
        node->setString("id", id);
        node->setString("name", name);
        node->setString("definition", definition);
        return node;
    }

    std::unique_ptr<StubDataNode> makeRosterRoot() const
    {
        auto root = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> characters;
        characters.push_back(this->makeCharacter("fighter_01", "Ryu", "assets/characters/fighter_01.json"));
        characters.push_back(this->makeCharacter("fighter_02", "Ken", "assets/characters/fighter_02.json"));
        root->setArray("characters", std::move(characters));
        return root;
    };
};

TEST_CASE_METHOD(CharacterRosterLoaderFixture, "CharacterRosterLoader parses characters and forwards path",
    "[unit][character_roster_loader]"
) {
    StubDataParser parser;
    parser.registerNode("assets/rosters/roster.json", this->makeRosterRoot());

    CharacterRosterLoader loader(parser);
    const auto roster = loader.load("assets/rosters/roster.json");

    REQUIRE(roster.getAll().size() == 2);
    const auto* ken = roster.findById("fighter_02");
    REQUIRE(ken != nullptr);
    REQUIRE(ken->name == "Ken");
    REQUIRE(ken->definitionPath == "assets/characters/fighter_02.json");
}
