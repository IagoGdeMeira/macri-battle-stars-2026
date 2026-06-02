#include "../../src/game/trigger_conditions/VelocityAboveCondition.h"

#include "../../src/domain/components/VelocityComponent.h"
#include "../../src/domain/components/StateComponent.h"
#include "../../src/domain/include/World/World.h"

#include <catch2/catch_test_macros.hpp>

class VelocityAboveConditionFixture
{
public:
    VelocityAboveConditionFixture()
    { this->world.components().registerComponent<VelocityComponent>(); }

    TriggerConditionContext makeContext()
    { return TriggerConditionContext { this->world, this->entity, this->state }; }

    void addVelocity(float vx, float vy)
    {
        this->world.components().add<VelocityComponent>(
            this->entity, VelocityComponent { vx, vy });
    }

    World world;
    StateComponent state;
    Entity entity { 1 };
};

TEST_CASE_METHOD(VelocityAboveConditionFixture, "VelocityAboveCondition compares speed magnitude",
    "[unit][velocity_above_condition]"
) {
    this->addVelocity(3.0f, 4.0f);

    REQUIRE(VelocityAboveCondition(5.0f).evaluate(this->makeContext()));
    REQUIRE_FALSE(VelocityAboveCondition(5.1f).evaluate(this->makeContext()));
}

TEST_CASE_METHOD(VelocityAboveConditionFixture, "VelocityAboveCondition fails without VelocityComponent",
    "[unit][velocity_above_condition]"
) {
    REQUIRE_FALSE(VelocityAboveCondition(1.0f).evaluate(this->makeContext()));
}
