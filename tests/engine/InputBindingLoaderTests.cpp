#include "../../src/engine/include/InputBindingLoader/InputBindingLoader.h"

#include "../../src/engine/include/DataNode/DataNode.h"
#include "../../src/engine/include/DataParser/DataParser.h"
#include "../../src/engine/include/InputMapper/InputMapper.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class InputBindingLoaderFixture
{
public:
    class StubNode : public DataNode
    {
    public:
        void setString(const std::string& key, const std::string& value)
        { this->strings[key] = value; }

        void setInt(const std::string& key, int value)
        { this->ints[key] = value; }

        void setFloat(const std::string& key, float value)
        { this->floats[key] = value; }

        void setArray(const std::string& key, std::vector<std::unique_ptr<DataNode>> value)
        {
            std::vector<StubNode> converted;
            converted.reserve(value.size());

            for (auto& node : value)
            {
                auto* typed = dynamic_cast<StubNode*>(node.get());
                if (!typed) throw std::runtime_error("Unexpected node type in fake tree");
                converted.push_back(*typed);
            }

            this->arrays[key] = std::move(converted);
        }

        bool has(const std::string& key) const override
        {
            return this->strings.contains(key)
                || this->ints.contains(key)
                || this->floats.contains(key)
                || this->bools.contains(key)
                || this->arrays.contains(key);
        }

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
            { out.push_back(std::make_unique<StubNode>(node)); }

            return out;
        }
    private:
        std::unordered_map<std::string, std::string> strings;
        std::unordered_map<std::string, int> ints;
        std::unordered_map<std::string, float> floats;
        std::unordered_map<std::string, bool> bools;
        std::unordered_map<std::string, std::vector<StubNode>> arrays;
    };

    class StubParser : public DataParser
    {
    public:
        explicit StubParser(std::unique_ptr<DataNode> rootNode) : root(std::move(rootNode)) {}

        mutable std::string lastPath;

        std::unique_ptr<DataNode> parse(const std::string& filePath) const override
        {
            this->lastPath = filePath;

            auto* typed = dynamic_cast<StubNode*>(root.get());
            if (!typed) throw std::runtime_error("Unexpected root node type in fake parser");
            
            return std::make_unique<StubNode>(dynamic_cast<StubNode&>(*root));
        }

        static std::unique_ptr<DataNode> makeBindingsRoot()
        {
            auto makeBinding = [](const std::string& action, const std::string& source)
            {
                auto binding = std::make_unique<StubNode>();
                binding->setString("action", action);
                binding->setString("source", source);
                return binding;
            };

            auto player1 = std::make_unique<StubNode>();
            player1->setInt("id", 1);

            std::vector<std::unique_ptr<DataNode>> player1Bindings;
            player1Bindings.push_back(makeBinding("Punch", "Keyboard.A"));
            player1Bindings.push_back(makeBinding("Jump", "Keyboard.Space"));
            player1->setArray("bindings", std::move(player1Bindings));

            auto player2 = std::make_unique<StubNode>();
            player2->setInt("id", 2);

            std::vector<std::unique_ptr<DataNode>> player2Bindings;
            player2Bindings.push_back(makeBinding("Defend", "Keyboard.LShift"));
            player2->setArray("bindings", std::move(player2Bindings));

            auto root = std::make_unique<StubNode>();
            std::vector<std::unique_ptr<DataNode>> players;
            players.push_back(std::move(player1));
            players.push_back(std::move(player2));
            root->setArray("players", std::move(players));

            return root;
        }

    private:
        std::unique_ptr<DataNode> root;
    };
};

TEST_CASE_METHOD(InputBindingLoaderFixture, "InputBindingLoader builds context for all players",
    "[unit][input_binding_loader]"
) {
    StubParser parser(StubParser::makeBindingsRoot());
    InputBindingLoader loader(parser);

    const auto context = loader.load("assets/inputs/input_bindings.json");

    REQUIRE(context.bindings.size() == 2);
    REQUIRE(context.bindings.at(1).keyMap.at(InputSource::keyboard(KeyCode::A)) == InputAction::Punch);
    REQUIRE(context.bindings.at(1).keyMap.at(InputSource::keyboard(KeyCode::Space)) == InputAction::Jump);
    REQUIRE(context.bindings.at(2).keyMap.at(InputSource::keyboard(KeyCode::LShift)) == InputAction::Defend);
}

TEST_CASE_METHOD(InputBindingLoaderFixture, "InputBindingLoader forwards path to parser",
    "[unit][input_binding_loader]"
) {
    StubParser parser(StubParser::makeBindingsRoot());
    InputBindingLoader loader(parser);

    (void)loader.load("custom/path/bindings.json");

    REQUIRE(parser.lastPath == "custom/path/bindings.json");
}

TEST_CASE_METHOD(InputBindingLoaderFixture, "InputBindingLoader handles empty players list",
    "[unit][input_binding_loader]"
) {
    auto root = std::make_unique<StubNode>();
    root->setArray("players", {});

    StubParser parser(std::move(root));
    InputBindingLoader loader(parser);

    const auto context = loader.load("ignored.json");

    REQUIRE(context.bindings.empty());
}

TEST_CASE_METHOD(InputBindingLoaderFixture, "InputBindingLoader keeps unknown mapped values as fallback enums",
    "[unit][input_binding_loader]"
) {
    auto unknownBinding = std::make_unique<StubNode>();
    unknownBinding->setString("action", "NotAnAction");
    unknownBinding->setString("source", "Keyboard.NotAKey");

    auto player = std::make_unique<StubNode>();
    player->setInt("id", 17);

    std::vector<std::unique_ptr<DataNode>> bindings;
    bindings.push_back(std::move(unknownBinding));
    player->setArray("bindings", std::move(bindings));

    auto root = std::make_unique<StubNode>();
    std::vector<std::unique_ptr<DataNode>> players;
    players.push_back(std::move(player));
    root->setArray("players", std::move(players));

    StubParser parser(std::move(root));
    InputBindingLoader loader(parser);

    const auto context = loader.load("ignored.json");

    const auto& keyMap = context.bindings.at(17).keyMap;
    REQUIRE(keyMap.contains(InputSource::keyboard(KeyCode::Unknown)));
    REQUIRE(keyMap.at(InputSource::keyboard(KeyCode::Unknown)) == InputAction::None);
    REQUIRE(InputMapper::keyToString(KeyCode::Unknown) == "Unknown");
    REQUIRE(InputMapper::actionToString(InputAction::None) == "None");
}
