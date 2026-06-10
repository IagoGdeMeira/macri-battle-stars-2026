#include "../../../src/game/trigger_conditions/MinTimeCondition.h"

#include "../../../src/domain/components/StateComponent.h"
#include "../../../src/domain/include/World/World.h"

#include <catch2/catch_test_macros.hpp>

class MinTimeConditionFixture
{
public:
    World world;
    StateComponent state;
    Entity entity{1};

    TriggerConditionContext makeContext()
    { return TriggerConditionContext { this->world, this->entity, this->state }; }
};

TEST_CASE_METHOD(MinTimeConditionFixture, "MinTimeCondition compares elapsed state time",
    "[unit][min_time_condition]"
) {
    this->state.timeInState = 0.75f;

    REQUIRE(MinTimeCondition(0.5f).evaluate(this->makeContext()));
    REQUIRE_FALSE(MinTimeCondition(1.f).evaluate(this->makeContext()));
}
