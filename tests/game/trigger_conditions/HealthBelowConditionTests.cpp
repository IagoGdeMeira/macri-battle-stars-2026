#include "../../../src/game/trigger_conditions/HealthBelowCondition.h"

#include "../../../src/domain/components/HealthComponent.h"
#include "../../../src/domain/components/StateComponent.h"
#include "../../../src/domain/include/World/World.h"

#include <catch2/catch_test_macros.hpp>

class HealthBelowConditionFixture
{
public:
    HealthBelowConditionFixture()
    { this->world.components().registerComponent<HealthComponent>(); }

    TriggerConditionContext makeContext()
    { return TriggerConditionContext { this->world, this->entity, this->state }; }

    void addHealth(int current, int max = 100)
    {
        this->world.components().add<HealthComponent>(
            this->entity, HealthComponent { current, max });
    }

    World world;
    StateComponent state;
    Entity entity { 1 };
};

TEST_CASE_METHOD(HealthBelowConditionFixture, "HealthBelowCondition compares current health",
    "[unit][health_below_condition]"
) {
    this->addHealth(40, 100);

    REQUIRE(HealthBelowCondition(50).evaluate(this->makeContext()));
    REQUIRE_FALSE(HealthBelowCondition(30).evaluate(this->makeContext()));
}

TEST_CASE_METHOD(HealthBelowConditionFixture, "HealthBelowCondition fails without HealthComponent",
    "[unit][health_below_condition]"
) {
    REQUIRE_FALSE(HealthBelowCondition(50).evaluate(this->makeContext()));
}
