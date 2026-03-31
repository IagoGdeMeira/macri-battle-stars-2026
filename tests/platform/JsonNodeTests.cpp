#include "../../src/platform/include/JsonNode/JsonNode.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("JsonNode reports existing keys",
    "[unit][json_node]"
) {
    JsonNode node(json{{"name", "Ryu"}, {"hp", 100}, {"speed", 3.5f}});

    REQUIRE(node.has("name"));
    REQUIRE(node.has("hp"));
    REQUIRE(node.has("speed"));
    REQUIRE_FALSE(node.has("missing"));
}

TEST_CASE("JsonNode reads primitive values",
    "[unit][json_node]"
) {
    JsonNode node(json{{"name", "Ken"}, {"hp", 90}, {"speed", 4.25f}});

    REQUIRE(node.getString("name") == "Ken");
    REQUIRE(node.getInt("hp") == 90);
    REQUIRE(node.getFloat("speed") == 4.25f);
}

TEST_CASE("JsonNode throws when key does not exist",
    "[unit][json_node]"
) {
    JsonNode node(json{{"name", "Chun-Li"}});

    REQUIRE_THROWS_AS(node.getString("unknown"), std::runtime_error);
    REQUIRE_THROWS_AS(node.getInt("unknown"), std::runtime_error);
    REQUIRE_THROWS_AS(node.getFloat("unknown"), std::runtime_error);
    REQUIRE_THROWS_AS(node.getArray("unknown"), std::runtime_error);
}

TEST_CASE("JsonNode returns nested array as DataNode collection",
    "[unit][json_node]"
) {
    JsonNode node(json{
        {"players", json::array({
            json{{"id", 1}, {"name", "P1"}},
            json{{"id", 2}, {"name", "P2"}}
        })}
    });

    const auto players = node.getArray("players");

    REQUIRE(players.size() == 2);
    REQUIRE(players[0]->getInt("id") == 1);
    REQUIRE(players[0]->getString("name") == "P1");
    REQUIRE(players[1]->getInt("id") == 2);
    REQUIRE(players[1]->getString("name") == "P2");
}
