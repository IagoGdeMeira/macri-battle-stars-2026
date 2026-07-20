#include "platform/include/JsonNode/JsonNode.h"

#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

TEST_CASE("JsonNode reports existing keys", "[unit][json_node]")
{
    JsonNode node(json{{"name", "Ryu"}, {"hp", 100}, {"speed", 3.5f}});

    REQUIRE(node.has("name"));
    REQUIRE(node.has("hp"));
    REQUIRE(node.has("speed"));
    REQUIRE_FALSE(node.has("missing"));
}

TEST_CASE("JsonNode reads primitive values", "[unit][json_node]")
{
    JsonNode node(json{{"name", "Ken"}, {"hp", 90}, {"speed", 4.25f}, {"alive", true}});

    REQUIRE(node.getString("name") == "Ken");
    REQUIRE(node.getInt("hp") == 90);
    REQUIRE(node.getFloat("speed") == 4.25f);
    REQUIRE(node.getBool("alive") == true);
}

TEST_CASE("JsonNode reads root primitive values", "[unit][json_node]")
{
    JsonNode stringNode(json("Ryu"));
    JsonNode intNode(json(42));
    JsonNode floatNode(json(3.5));
    JsonNode boolNode(json(true));

    REQUIRE(stringNode.getString("") == "Ryu");
    REQUIRE(intNode.getInt("") == 42);
    REQUIRE(floatNode.getFloat("") == 3.5f);
    REQUIRE(boolNode.getBool("") == true);
}

TEST_CASE("JsonNode returns fallback when key does not exist", "[unit][json_node]")
{
    JsonNode node(json{{"name", "Chun-Li"}});
    const std::string stringFallback = "fallback";
    const int intFallback = 123;
    const float floatFallback = 4.5f;
    const bool boolFallback = true;

    REQUIRE(node.getString("unknown", stringFallback) == "fallback");
    REQUIRE(node.getInt("unknown", intFallback) == 123);
    REQUIRE(node.getFloat("unknown", floatFallback) == 4.5f);
    REQUIRE(node.getBool("unknown", boolFallback) == true);
    REQUIRE_THROWS_AS(node.getArray("unknown"), std::runtime_error);
}

TEST_CASE("JsonNode returns nested array as DataNode collection", "[unit][json_node]")
{
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

TEST_CASE("JsonNode returns nested object as DataNode", "[unit][json_node]")
{
    JsonNode node(json{{"player", json{{"id", 1}, {"name", "P1"}}}});

    const auto player = node.getObject("player");

    REQUIRE(player->getInt("id") == 1);
    REQUIRE(player->getString("name") == "P1");
}

TEST_CASE("JsonNode throws when accessing wrong type", "[unit][json_node]")
{
    JsonNode node(json{{"name", "Ryu"}, {"hp", 100}});

    REQUIRE_THROWS_AS(node.getInt("name"), std::runtime_error);
    REQUIRE_THROWS_AS(node.getString("hp"), std::runtime_error);
    REQUIRE_THROWS_AS(node.getFloat("name"), std::runtime_error);
}

TEST_CASE("JsonNode returns fallback when accessing non-existent key without explicit fallback",
    "[unit][json_node]"
) {
    JsonNode node(json{{"name", "Ryu"}});

    REQUIRE(node.getString("unknown") == "");
    REQUIRE(node.getInt("unknown") == 0);
    REQUIRE(node.getFloat("unknown") == 0.f);
    REQUIRE(node.getBool("unknown") == false);
    REQUIRE_THROWS_AS(node.getArray("unknown"), std::runtime_error);
    REQUIRE(node.getObject("unknown") == nullptr);
}

TEST_CASE("JsonNode throws when accessing non-existent key as array or object", "[unit][json_node]")
{
    JsonNode node(json{{"name", "Ryu"}});

    REQUIRE_THROWS_AS(node.getArray("unknown"), std::runtime_error);

    auto obj = node.getObject("unknown");
    REQUIRE(obj == nullptr);
}

TEST_CASE("JsonNode throws when accessing non-array as array", "[unit][json_node]")
{
    JsonNode node(json{{"name", "Ryu"}});
    REQUIRE_THROWS_AS(node.getArray("name"), std::runtime_error);
}

TEST_CASE("JsonNode returns nullptr when accessing non-object as object", "[unit][json_node]")
{
    JsonNode node(json{{"name", "Ryu"}});
    auto obj = node.getObject("name");
    REQUIRE(obj == nullptr);
}

TEST_CASE("JsonNode allows setting primitive values", "[unit][json_node]")
{
    JsonNode node(json{});

    node.setString("name", "Ryu");
    node.setInt("hp", 100);
    node.setFloat("speed", 3.5f);
    node.setBool("alive", true);

    REQUIRE(node.getString("name") == "Ryu");
    REQUIRE(node.getInt("hp") == 100);
    REQUIRE(node.getFloat("speed") == 3.5f);
    REQUIRE(node.getBool("alive") == true);
}

TEST_CASE("JsonNode allows setting nested array and object", "[unit][json_node]")
{
    JsonNode node(json{});

    std::vector<std::unique_ptr<DataNode>> players;
    auto player1 = std::make_unique<JsonNode>(json{{"id", 1}, {"name", "P1"}});
    auto player2 = std::make_unique<JsonNode>(json{{"id", 2}, {"name", "P2"}});
    players.push_back(std::move(player1));
    players.push_back(std::move(player2));
    node.setArray("players", std::move(players));

    auto settings = std::make_unique<JsonNode>(json{{"volume", 80}, {"fullscreen", true}});
    node.setObject("settings", std::move(settings));

    const auto retrievedPlayers = node.getArray("players");
    REQUIRE(retrievedPlayers.size() == 2);
    REQUIRE(retrievedPlayers[0]->getInt("id") == 1);
    REQUIRE(retrievedPlayers[0]->getString("name") == "P1");
    REQUIRE(retrievedPlayers[1]->getInt("id") == 2);
    REQUIRE(retrievedPlayers[1]->getString("name") == "P2");

    const auto retrievedSettings = node.getObject("settings");
    REQUIRE(retrievedSettings->getInt("volume") == 80);
    REQUIRE(retrievedSettings->getBool("fullscreen") == true);
}

TEST_CASE("JsonNode clone creates a deep copy", "[unit][json_node]")
{
    JsonNode node(json{{"name", "Ryu"}, {"hp", 100}, {"settings", json{{"volume", 80}}}});

    auto clonedNode = node.clone();

    REQUIRE(clonedNode->getString("name") == "Ryu");
    REQUIRE(clonedNode->getInt("hp") == 100);
    REQUIRE(clonedNode->getObject("settings")->getInt("volume") == 80);

    node.setString("name", "Ken");
    node.setInt("hp", 90);
    node.getObject("settings")->setInt("volume", 50);

    REQUIRE(clonedNode->getString("name") == "Ryu");
    REQUIRE(clonedNode->getInt("hp") == 100);
    REQUIRE(clonedNode->getObject("settings")->getInt("volume") == 80);
}
