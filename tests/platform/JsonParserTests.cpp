#include "../../src/platform/include/JsonParser/JsonParser.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

TEST_CASE("JsonParser parses valid JSON file", "[integration][json_parser]")
{
    const auto path = std::filesystem::temp_directory_path() / "macri_json_parser_valid_test.json";

    {
        std::ofstream out(path);
        out << R"({"game":"Macri","players":[{"id":1}]})";
    }

    JsonParser parser;
    const auto root = parser.parse(path.string());

    REQUIRE(root->getString("game") == "Macri");

    const auto players = root->getArray("players");
    REQUIRE(players.size() == 1);
    REQUIRE(players[0]->getInt("id") == 1);

    std::filesystem::remove(path);
}

TEST_CASE("JsonParser throws when file cannot be opened", "[integration][json_parser]")
{
    JsonParser parser;
    REQUIRE_THROWS_AS(parser.parse("file/that/does/not/exist.json"), std::runtime_error);
}

TEST_CASE("JsonParser throws when JSON is invalid", "[integration][json_parser]")
{
    const auto path = std::filesystem::temp_directory_path() / "macri_json_parser_invalid_test.json";

    {
        std::ofstream out(path);
        out << R"({"broken": [})";
    }

    JsonParser parser;
    REQUIRE_THROWS(parser.parse(path.string()));

    std::filesystem::remove(path);
}
