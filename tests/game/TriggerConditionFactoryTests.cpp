#include "../../src/game/include/TriggerConditionFactory/TriggerConditionFactory.h"

#include "../stubs/StubDataNode.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <stdexcept>
#include <string>

class TriggerConditionFactoryFixture
{
public:
    StubDataNode node;
};

TEST_CASE_METHOD(TriggerConditionFactoryFixture, "TriggerConditionFactory creates min_time conditions",
    "[unit][trigger_condition_factory]"
) {
    this->node.setString("type", "min_time");
    this->node.setFloat("value", 0.5f);
    auto condition = TriggerConditionFactory::create(this->node);
    REQUIRE(condition != nullptr);
}

TEST_CASE_METHOD(TriggerConditionFactoryFixture, "TriggerConditionFactory creates input_pressed conditions",
    "[unit][trigger_condition_factory]"
) {
    this->node.setString("type", "input_pressed");
    this->node.setString("action", "Punch");
    auto condition = TriggerConditionFactory::create(this->node);
    REQUIRE(condition != nullptr);
}

TEST_CASE_METHOD(TriggerConditionFactoryFixture, "TriggerConditionFactory rejects unknown condition types",
    "[unit][trigger_condition_factory]"
) {
    this->node.setString("type", "unknown");
    REQUIRE_THROWS_AS(TriggerConditionFactory::create(this->node), std::runtime_error);
}
