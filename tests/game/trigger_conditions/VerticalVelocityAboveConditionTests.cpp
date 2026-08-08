#include "game/trigger_conditions/VerticalVelocityAboveCondition.h"

#include "domain/components/StateComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/World/World.h"

#include "game/value_objects/TriggerConditionContext/TriggerConditionContext.h"

#include <catch2/catch_test_macros.hpp>

class VerticalVelocityAboveConditionFixture
{
public:
    World world;
    StateComponent state;
    Entity entity{1};

    VerticalVelocityAboveConditionFixture()
    { this->world.components().registerComponent<VelocityComponent>(); }

    TriggerConditionContext makeContext()
    { return TriggerConditionContext{ this->world, this->entity, this->state }; }

    void addVelocity(float vx, float vy)
    { this->world.components().add<VelocityComponent>(this->entity, VelocityComponent{vx, vy}); }
};

TEST_CASE_METHOD(VerticalVelocityAboveConditionFixture, "VerticalVelocityAboveCondition returns true when velocity.y is above threshold",
    "[unit][vertical_velocity_above_condition]")
{
    VerticalVelocityAboveCondition condition(0.f);
    this->addVelocity(2.f, 5.f);
    REQUIRE(condition.evaluate(this->makeContext()));
}

TEST_CASE_METHOD(VerticalVelocityAboveConditionFixture, "VerticalVelocityAboveCondition returns false when velocity.y is below threshold",
    "[unit][vertical_velocity_above_condition]")
{
    VerticalVelocityAboveCondition condition(0.f);
    this->addVelocity(2.f, -3.0f);
    REQUIRE_FALSE(condition.evaluate(this->makeContext()));
}

TEST_CASE_METHOD(VerticalVelocityAboveConditionFixture, "VerticalVelocityAboveCondition returns false when VelocityComponent is missing",
    "[unit][vertical_velocity_above_condition]")
{
    VerticalVelocityAboveCondition condition(0.f);
    REQUIRE_FALSE(condition.evaluate(this->makeContext()));
}

TEST_CASE_METHOD(VerticalVelocityAboveConditionFixture, "VerticalVelocityAboveCondition returns false when velocity.y equals threshold",
    "[unit][vertical_velocity_above_condition]")
{
    VerticalVelocityAboveCondition condition(1.5f);
    this->addVelocity(0.f, 1.5f);
    REQUIRE_FALSE(condition.evaluate(this->makeContext()));
}
