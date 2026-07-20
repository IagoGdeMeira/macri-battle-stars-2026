#include "engine/include/InputBindingLoader/InputBindingLoader.h"

#include "StubDataNode.h"
#include "StubDataParser.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <vector>

class InputBindingLoaderFixture
{
public:
    std::unique_ptr<StubDataNode> makeBinding(const std::string& action, const std::string& source) const
    {
        auto binding = std::make_unique<StubDataNode>();
        binding->setString("action", action);
        binding->setString("source", source);
        return binding;
    }

    std::unique_ptr<StubDataNode> makePlayerBindings(int playerId, std::vector<std::unique_ptr<StubDataNode>> bindings) const
    {
        auto player = std::make_unique<StubDataNode>();
        player->setInt("id", playerId);
        std::vector<std::unique_ptr<DataNode>> bindingsVec;
        for (auto& b : bindings) bindingsVec.push_back(std::move(b));
        player->setArray("bindings", std::move(bindingsVec));
        return player;
    }

    std::unique_ptr<StubDataNode> makeBindingsRoot() const
    {
        auto root = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> players;

        std::vector<std::unique_ptr<StubDataNode>> player1Bindings;
        player1Bindings.push_back(this->makeBinding("Punch", "Keyboard.A"));
        player1Bindings.push_back(this->makeBinding("Jump", "Keyboard.Space"));
        auto player1 = this->makePlayerBindings(1, std::move(player1Bindings));
        players.push_back(std::move(player1));

        std::vector<std::unique_ptr<StubDataNode>> player2Bindings;
        player2Bindings.push_back(this->makeBinding("Defend", "Keyboard.LShift"));
        auto player2 = this->makePlayerBindings(2, std::move(player2Bindings));
        players.push_back(std::move(player2));

        root->setArray("players", std::move(players));
        return root;
    }
};

TEST_CASE_METHOD(InputBindingLoaderFixture, "InputBindingLoader builds context for all players",
    "[unit][input_binding_loader]"
) {
    StubDataParser parser;
    parser.registerNode("bindings.json", this->makeBindingsRoot());

    InputBindingLoader loader(parser);
    const auto context = loader.load("bindings.json");

    REQUIRE(context.bindings.size() == 2);
    REQUIRE(context.bindings.at(1).keyMap.at(InputSource::keyboard(KeyCode::A)) == InputAction::Punch);
    REQUIRE(context.bindings.at(1).keyMap.at(InputSource::keyboard(KeyCode::Space)) == InputAction::Jump);
    REQUIRE(context.bindings.at(2).keyMap.at(InputSource::keyboard(KeyCode::LShift)) == InputAction::Defend);
}

TEST_CASE_METHOD(InputBindingLoaderFixture, "InputBindingLoader forwards path to parser",
    "[unit][input_binding_loader]"
) {
    StubDataParser parser;
    parser.registerNode("custom/path/bindings.json", this->makeBindingsRoot());

    InputBindingLoader loader(parser);
    REQUIRE_NOTHROW(loader.load("custom/path/bindings.json"));
}

TEST_CASE_METHOD(InputBindingLoaderFixture, "InputBindingLoader handles empty players list",
    "[unit][input_binding_loader]"
) {
    auto emptyRoot = std::make_unique<StubDataNode>();
    emptyRoot->setArray("players", {});

    StubDataParser parser;
    parser.registerNode("empty.json", std::move(emptyRoot));

    InputBindingLoader loader(parser);
    const auto context = loader.load("empty.json");
    REQUIRE(context.bindings.empty());
}

TEST_CASE_METHOD(InputBindingLoaderFixture, "InputBindingLoader keeps unknown mapped values as fallback enums",
    "[unit][input_binding_loader]"
) {
    auto unknownBinding = std::make_unique<StubDataNode>();
    unknownBinding->setString("action", "NotAnAction");
    unknownBinding->setString("source", "Keyboard.NotAKey");

    auto player = std::make_unique<StubDataNode>();
    player->setInt("id", 17);
    std::vector<std::unique_ptr<DataNode>> bindings;
    bindings.push_back(std::move(unknownBinding));
    player->setArray("bindings", std::move(bindings));

    auto root = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> players;
    players.push_back(std::move(player));
    root->setArray("players", std::move(players));

    StubDataParser parser;
    parser.registerNode("unknown.json", std::move(root));

    InputBindingLoader loader(parser);
    const auto context = loader.load("unknown.json");

    const auto& keyMap = context.bindings.at(17).keyMap;
    REQUIRE(keyMap.contains(InputSource::keyboard(KeyCode::Unknown)));
    REQUIRE(keyMap.at(InputSource::keyboard(KeyCode::Unknown)) == InputAction::None);
}
