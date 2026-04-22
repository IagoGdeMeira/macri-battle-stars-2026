#include "../../src/game/include/StateMachineLoader/StateMachineLoader.h"

#include "../../src/domain/components/StateComponent.h"
#include "../../src/domain/include/Entity/Entity.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/DataNode/DataNode.h"
#include "../../src/engine/include/DataParser/DataParser.h"

#include "../../src/game/include/ConditionContext/ConditionContext.h"
#include "../../src/game/include/StateIdMapper/StateIdMapper.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class StateMachineLoaderFixture
{
public:
    class Node : public DataNode
    {
    public:
        void setString(const std::string& key, const std::string& value)
        { this->strings[key] = value; }

        void setInt(const std::string& key, int value)
        { this->ints[key] = value; }

        void setFloat(const std::string& key, float value)
        { this->floats[key] = value; }

        void setBool(const std::string& key, bool value)
        { this->bools[key] = value; }

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
            { out.push_back(std::make_unique<Node>(node)); }

            return out;
        }

    private:
        std::unordered_map<std::string, std::string> strings;
        std::unordered_map<std::string, int> ints;
        std::unordered_map<std::string, float> floats;
        std::unordered_map<std::string, bool> bools;
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

    Node root;

    std::unique_ptr<DataNode> makeMachineRoot()
    {
        auto triggerA = std::make_unique<Node>();
        triggerA->setString("", "Punched");

        auto triggerB = std::make_unique<Node>();
        triggerB->setString("", "Kicked");

        auto condition = std::make_unique<Node>();
        condition->setString("type", "min_time");
        condition->setFloat("value", 0.5f);

        auto transitionA = std::make_unique<Node>();
        transitionA->setString("from", "Idle");
        transitionA->setString("to", "Punching");
        transitionA->setInt("priority", 7);

        std::vector<std::unique_ptr<DataNode>> triggersA;
        triggersA.push_back(std::move(triggerA));
        triggersA.push_back(std::move(triggerB));
        transitionA->setArray("triggers", std::move(triggersA));

        std::vector<std::unique_ptr<DataNode>> conditionsA;
        conditionsA.push_back(std::move(condition));
        transitionA->setArray("conditions", std::move(conditionsA));

        auto transitionB = std::make_unique<Node>();
        transitionB->setString("from", "Punching");
        transitionB->setString("to", "Idle");
        transitionB->setString("trigger", "Landed");

        auto rootNode = std::make_unique<Node>();
        std::vector<std::unique_ptr<DataNode>> transitions;
        transitions.push_back(std::move(transitionA));
        transitions.push_back(std::move(transitionB));
        rootNode->setArray("transitions", std::move(transitions));

        return rootNode;
    }
};

TEST_CASE_METHOD(StateMachineLoaderFixture, "StateMachineLoader parses transitions and trigger collections",
    "[unit][state_machine_loader]"
) {
    Parser parser(this->makeMachineRoot());
    StateMachineLoader loader(parser);

    const auto machine = loader.load("assets/fsm/state_machine.json");

    REQUIRE(parser.lastPath == "assets/fsm/state_machine.json");
    REQUIRE(machine.transitions.size() == 2);

    REQUIRE(machine.transitions[0].from == StateId::Idle);
    REQUIRE(machine.transitions[0].to == StateId::Punching);
    REQUIRE(machine.transitions[0].priority == 7);
    REQUIRE(machine.transitions[0].triggers.size() == 2);
    REQUIRE(machine.transitions[0].triggers[0] == TriggerId::Punched);
    REQUIRE(machine.transitions[0].triggers[1] == TriggerId::Kicked);
    REQUIRE(machine.transitions[0].conditions.size() == 1);

    World world;
    Entity entity = world.entities().create();
    StateComponent state;
    state.timeInState = 0.75f;

    ConditionContext ctx { world, entity, state };
    REQUIRE(machine.transitions[0].conditions[0]->evaluate(ctx));

    REQUIRE(machine.transitions[1].from == StateId::Punching);
    REQUIRE(machine.transitions[1].to == StateId::Idle);
    REQUIRE(machine.transitions[1].priority == 0);
    REQUIRE(machine.transitions[1].triggers.size() == 1);
    REQUIRE(machine.transitions[1].triggers[0] == TriggerId::Landed);
}

TEST_CASE_METHOD(StateMachineLoaderFixture, "StateMachineLoader rejects transitions without triggers",
    "[unit][state_machine_loader]"
) {
    auto transition = std::make_unique<Node>();
    transition->setString("from", "Idle");
    transition->setString("to", "Punching");

    auto rootNode = std::make_unique<Node>();
    std::vector<std::unique_ptr<DataNode>> transitions;
    transitions.push_back(std::move(transition));
    rootNode->setArray("transitions", std::move(transitions));

    Parser parser(std::move(rootNode));
    StateMachineLoader loader(parser);

    REQUIRE_THROWS_AS(loader.load("assets/fsm/state_machine.json"), std::runtime_error);
}

TEST_CASE_METHOD(StateMachineLoaderFixture, "StateMachineLoader resolves custom states with mapper",
    "[unit][state_machine_loader]"
) {
    auto transition = std::make_unique<Node>();
    transition->setString("from", "Idle");
    transition->setString("to", "PowerCharge");
    transition->setString("trigger", "Punched");

    auto rootNode = std::make_unique<Node>();
    std::vector<std::unique_ptr<DataNode>> transitions;
    transitions.push_back(std::move(transition));
    rootNode->setArray("transitions", std::move(transitions));

    Parser parser(std::move(rootNode));
    StateMachineLoader loader(parser);

    StateIdMapper mapper;
    const auto powerCharge = mapper.addCustomMapping("PowerCharge");

    const auto machine = loader.load("assets/fsm/custom_state_machine.json", mapper);

    REQUIRE(machine.transitions.size() == 1);
    REQUIRE(machine.transitions[0].from == StateId::Idle);
    REQUIRE(machine.transitions[0].to == powerCharge);
    REQUIRE(machine.transitions[0].triggers.size() == 1);
    REQUIRE(machine.transitions[0].triggers[0] == TriggerId::Punched);
}

TEST_CASE_METHOD(StateMachineLoaderFixture, "StateMachineLoader with mapper rejects unmapped custom state",
    "[unit][state_machine_loader]"
) {
    auto transition = std::make_unique<Node>();
    transition->setString("from", "Idle");
    transition->setString("to", "PowerCharge");
    transition->setString("trigger", "Punched");

    auto rootNode = std::make_unique<Node>();
    std::vector<std::unique_ptr<DataNode>> transitions;
    transitions.push_back(std::move(transition));
    rootNode->setArray("transitions", std::move(transitions));

    Parser parser(std::move(rootNode));
    StateMachineLoader loader(parser);

    StateIdMapper mapper;

    REQUIRE_THROWS_AS(loader.load("assets/fsm/custom_state_machine_bad.json", mapper), std::runtime_error);
}
