#include "game/include/TriggerBindingLoader/TriggerBindingLoader.h"

#include "StubDataNode.h"
#include "StubDataParser.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <vector>

class TriggerBindingLoaderFixture
{
public:
    std::unique_ptr<StubDataNode> makeTriggerBinding(
        const std::string& action, const std::vector<std::string>& triggers
    ) const {
        auto binding = std::make_unique<StubDataNode>();
        binding->setString("action", action);
        std::vector<std::unique_ptr<DataNode>> triggerNodes;
        for (const auto& t : triggers)
        {
            auto tn = std::make_unique<StubDataNode>();
            tn->setString("trigger", t);
            triggerNodes.push_back(std::move(tn));
        }
        binding->setArray("triggers", std::move(triggerNodes));
        return binding;
    }

    std::unique_ptr<StubDataNode> makeBindingsRoot() const
    {
        auto root = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> bindings;
        bindings.push_back(this->makeTriggerBinding("Jump", {"Jumped", "TouchedWall"}));
        bindings.push_back(this->makeTriggerBinding("Punch", {"Punched"}));
        bindings.push_back(this->makeTriggerBinding("Kick", {"Kicked"}));
        root->setArray("bindings", std::move(bindings));
        return root;
    }
};

TEST_CASE_METHOD(TriggerBindingLoaderFixture, "TriggerBindingLoader builds trigger bindings",
    "[unit][trigger_binding_loader]"
) {
    StubDataParser parser;
    parser.registerNode("triggers.json", this->makeBindingsRoot());

    TriggerBindingLoader loader(parser);
    const auto context = loader.load("triggers.json");

    REQUIRE(context.bindings.size() == 3);
    REQUIRE(context.bindings.at(InputAction::Jump).size() == 2);
    REQUIRE(context.bindings.at(InputAction::Jump)[0] == TriggerId::Jumped);
    REQUIRE(context.bindings.at(InputAction::Jump)[1] == TriggerId::TouchedWall);
    REQUIRE(context.bindings.at(InputAction::Punch).size() == 1);
    REQUIRE(context.bindings.at(InputAction::Punch)[0] == TriggerId::Punched);
    REQUIRE(context.bindings.at(InputAction::Kick).size() == 1);
    REQUIRE(context.bindings.at(InputAction::Kick)[0] == TriggerId::Kicked);
}

TEST_CASE_METHOD(TriggerBindingLoaderFixture, "TriggerBindingLoader forwards the file path to the parser",
    "[unit][trigger_binding_loader]"
) {
    StubDataParser parser;
    parser.registerNode("custom/path/triggers.json", this->makeBindingsRoot());

    TriggerBindingLoader loader(parser);
    REQUIRE_NOTHROW(loader.load("custom/path/triggers.json"));
}

TEST_CASE_METHOD(TriggerBindingLoaderFixture, "TriggerBindingLoader handles empty bindings",
    "[unit][trigger_binding_loader]"
) {
    auto emptyRoot = std::make_unique<StubDataNode>();
    emptyRoot->setArray("bindings", {});

    StubDataParser parser;
    parser.registerNode("empty.json", std::move(emptyRoot));

    TriggerBindingLoader loader(parser);
    const auto context = loader.load("empty.json");
    REQUIRE(context.bindings.empty());
}
