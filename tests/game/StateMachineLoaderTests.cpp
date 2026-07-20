#include "game/include/StateMachineLoader/StateMachineLoader.h"

#include "StubDataNode.h"
#include "StubDataParser.h"

#include "domain/components/StateComponent.h"
#include "domain/include/Entity/Entity.h"
#include "domain/include/World/World.h"

#include "game/include/StateIdMapper/StateIdMapper.h"
#include "game/value_objects/TriggerConditionContext/TriggerConditionContext.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <vector>

class StateMachineLoaderFixture
{
public:
    std::unique_ptr<StubDataNode> makeMachineRoot() const
    {
        auto triggerA = std::make_unique<StubDataNode>();
        triggerA->setString("", "Punched");
        auto triggerB = std::make_unique<StubDataNode>();
        triggerB->setString("", "Kicked");

        auto condition = std::make_unique<StubDataNode>();
        condition->setString("type", "min_time");
        condition->setFloat("value", 0.5f);

        auto transitionA = std::make_unique<StubDataNode>();
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

        auto transitionB = std::make_unique<StubDataNode>();
        transitionB->setString("from", "Punching");
        transitionB->setString("to", "Idle");
        transitionB->setString("trigger", "Landed");

        auto root = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> transitions;
        transitions.push_back(std::move(transitionA));
        transitions.push_back(std::move(transitionB));
        root->setArray("transitions", std::move(transitions));
        return root;
    }

    std::unique_ptr<StubDataNode> makeMachineRootWithoutTriggers() const
    {
        auto transition = std::make_unique<StubDataNode>();
        transition->setString("from", "Idle");
        transition->setString("to", "Punching");
        auto root = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> transitions;
        transitions.push_back(std::move(transition));
        root->setArray("transitions", std::move(transitions));
        return root;
    }

    std::unique_ptr<StubDataNode> makeMachineRootWithCustomState() const
    {
        auto transition = std::make_unique<StubDataNode>();
        transition->setString("from", "Idle");
        transition->setString("to", "PowerCharge");
        transition->setString("trigger", "Punched");
        auto root = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> transitions;
        transitions.push_back(std::move(transition));
        root->setArray("transitions", std::move(transitions));
        return root;
    }
};

TEST_CASE_METHOD(StateMachineLoaderFixture, "StateMachineLoader parses transitions and trigger collections",
    "[unit][state_machine_loader]"
) {
    StubDataParser parser;
    parser.registerNode("fsm.json", this->makeMachineRoot());

    StateMachineLoader loader(parser);
    const auto machine = loader.load("fsm.json");

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
    TriggerConditionContext ctx{ world, entity, state };
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
    StubDataParser parser;
    parser.registerNode("bad.json", this->makeMachineRootWithoutTriggers());

    StateMachineLoader loader(parser);
    REQUIRE_THROWS_AS(loader.load("bad.json"), std::runtime_error);
}

TEST_CASE_METHOD(StateMachineLoaderFixture, "StateMachineLoader resolves custom states with mapper",
    "[unit][state_machine_loader]"
) {
    StubDataParser parser;
    parser.registerNode("custom.json", this->makeMachineRootWithCustomState());

    StateMachineLoader loader(parser);
    StateIdMapper mapper;
    const auto powerCharge = mapper.addCustomMapping("PowerCharge");

    const auto machine = loader.load("custom.json", mapper);

    REQUIRE(machine.transitions.size() == 1);
    REQUIRE(machine.transitions[0].from == StateId::Idle);
    REQUIRE(machine.transitions[0].to == powerCharge);
    REQUIRE(machine.transitions[0].triggers.size() == 1);
    REQUIRE(machine.transitions[0].triggers[0] == TriggerId::Punched);
}

TEST_CASE_METHOD(StateMachineLoaderFixture, "StateMachineLoader with mapper rejects unmapped custom state",
    "[unit][state_machine_loader]"
) {
    StubDataParser parser;
    parser.registerNode("bad_custom.json", this->makeMachineRootWithCustomState());

    StateMachineLoader loader(parser);
    StateIdMapper mapper;
    REQUIRE_THROWS_AS(loader.load("bad_custom.json", mapper), std::runtime_error);
}
