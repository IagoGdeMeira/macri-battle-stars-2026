#include "../src/game/include/CharacterRosterLoader/CharacterRosterLoader.h"

#include "../src/engine/include/DataNode/DataNode.h"
#include "../src/engine/include/DataParser/DataParser.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class CharacterRosterLoaderFixture
{
public:
    class Node : public DataNode
    {
    public:
        void setString(const std::string& key, const std::string& value) { this->strings[key] = value; }

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
            (void)key;
            return fallback;
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

    static std::unique_ptr<DataNode> makeCharacter(const std::string& id, const std::string& name,
        const std::string& definition
    ) {
        auto character = std::make_unique<Node>();
        character->setString("id", id);
        character->setString("name", name);
        character->setString("definition", definition);
        return character;
    }

    static std::unique_ptr<DataNode> makeRosterRoot()
    {
        auto root = std::make_unique<Node>();

        std::vector<std::unique_ptr<DataNode>> characters;
        characters.push_back(makeCharacter("fighter_01", "Ryu", "assets/characters/fighter_01.json"));
        characters.push_back(makeCharacter("fighter_02", "Ken", "assets/characters/fighter_02.json"));
        root->setArray("characters", std::move(characters));

        return root;
    }
};

TEST_CASE_METHOD(CharacterRosterLoaderFixture, "CharacterRosterLoader parses characters and forwards path",
    "[unit][character_roster_loader]"
) {
    Parser parser(makeRosterRoot());
    CharacterRosterLoader loader(parser);

    const auto roster = loader.load("assets/maps/roster.json");

    REQUIRE(parser.lastPath == "assets/maps/roster.json");
    REQUIRE(roster.getAll().size() == 2);

    const auto* ken = roster.findById("fighter_02");
    REQUIRE(ken != nullptr);
    REQUIRE(ken->name == "Ken");
    REQUIRE(ken->definitionPath == "assets/characters/fighter_02.json");
}
