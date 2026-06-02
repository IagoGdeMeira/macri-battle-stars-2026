#include "../src/game/include/TriggerBindingLoader/TriggerBindingLoader.h"

#include "../src/domain/include/InputAction/InputAction.h"
#include "../src/domain/include/TriggerId/TriggerId.h"

#include "../src/engine/include/DataNode/DataNode.h"
#include "../src/engine/include/DataParser/DataParser.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class TriggerBindingLoaderFixture
{
public:
    class StubNode : public DataNode
    {
    public:
        void setString(const std::string& key, const std::string& value)
        { this->strings[key] = value; }

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
            auto makeTrigger = [](const std::string& triggerName)
            {
                auto trigger = std::make_unique<StubNode>();
                trigger->setString("", triggerName);
                return trigger;
            };

            auto makeBinding = [&](const std::string& action, std::vector<std::string> triggers)
            {
                auto binding = std::make_unique<StubNode>();
                binding->setString("action", action);

                std::vector<std::unique_ptr<DataNode>> triggerNodes;
                triggerNodes.reserve(triggers.size());

                for (const auto& triggerName : triggers)
                { triggerNodes.push_back(makeTrigger(triggerName)); }

                binding->setArray("triggers", std::move(triggerNodes));
                return binding;
            };

            auto root = std::make_unique<StubNode>();
            std::vector<std::unique_ptr<DataNode>> bindings;
            bindings.push_back(makeBinding("Jump", {"Jumped", "TouchedWall"}));
            bindings.push_back(makeBinding("Punch", {"Punched"}));
            bindings.push_back(makeBinding("Kick", {"Kicked"}));
            root->setArray("bindings", std::move(bindings));

            return root;
        }

    private:
        std::unique_ptr<DataNode> root;
    };
};

TEST_CASE_METHOD(TriggerBindingLoaderFixture, "TriggerBindingLoader builds trigger bindings",
    "[unit][trigger_binding_loader]"
) {
    StubParser parser(StubParser::makeBindingsRoot());
    TriggerBindingLoader loader(parser);

    const auto context = loader.load("assets/triggers/trigger_bindings.json");

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
    StubParser parser(StubParser::makeBindingsRoot());
    TriggerBindingLoader loader(parser);

    (void)loader.load("custom/path/trigger_bindings.json");

    REQUIRE(parser.lastPath == "custom/path/trigger_bindings.json");
}

TEST_CASE_METHOD(TriggerBindingLoaderFixture, "TriggerBindingLoader handles empty bindings",
    "[unit][trigger_binding_loader]"
) {
    auto root = std::make_unique<StubNode>();
    root->setArray("bindings", {});

    StubParser parser(std::move(root));
    TriggerBindingLoader loader(parser);

    const auto context = loader.load("ignored.json");

    REQUIRE(context.bindings.empty());
}
