#include "../../src/game/include/MapRosterLoader/MapRosterLoader.h"

#include "../stubs/StubDataNode.h"
#include "../stubs/StubDataParser.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <vector>

class MapRosterLoaderFixture
{
public:
    std::unique_ptr<StubDataNode> makeMapEntry(
        const std::string& id, const std::string& name, const std::string& definition
    ) const {
        auto entry = std::make_unique<StubDataNode>();
        entry->setString("id", id);
        entry->setString("name", name);
        entry->setString("definition", definition);
        return entry;
    }

    std::unique_ptr<StubDataNode> makeRosterRoot() const
    {
        auto root = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> maps;
        maps.push_back(makeMapEntry("dojo", "Dojo", "assets/maps/dojo.json"));
        maps.push_back(makeMapEntry("roof", "Rooftop", "assets/maps/roof.json"));
        root->setArray("maps", std::move(maps));
        return root;
    }
};

TEST_CASE_METHOD(MapRosterLoaderFixture, "MapRosterLoader parses maps and forwards path",
    "[unit][map_roster_loader]"
) {
    StubDataParser parser;
    parser.registerNode("assets/maps/roster.json", this->makeRosterRoot());

    MapRosterLoader loader(parser);
    const auto roster = loader.load("assets/maps/roster.json");

    REQUIRE(roster.getAll().size() == 2);

    const auto* roof = roster.findById("roof");
    REQUIRE(roof != nullptr);
    REQUIRE(roof->name == "Rooftop");
    REQUIRE(roof->definitionPath == "assets/maps/roof.json");
}
