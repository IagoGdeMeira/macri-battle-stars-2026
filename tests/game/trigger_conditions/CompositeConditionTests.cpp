#include "../../../src/game/trigger_conditions/CompositeCondition.h"

#include "../../../src/domain/components/HealthComponent.h"
#include "../../../src/domain/components/StateComponent.h"
#include "../../../src/domain/components/VelocityComponent.h"
#include "../../../src/domain/include/World/World.h"

#include "../../../src/game/trigger_conditions/HealthBelowCondition.h"
#include "../../../src/game/trigger_conditions/MinTimeCondition.h"
#include "../../../src/game/trigger_conditions/VelocityAboveCondition.h"

#include <catch2/catch_test_macros.hpp>

class CompositeConditionFixture
{
public:
    World world;
    StateComponent state;
    Entity entity{1};
    
    CompositeConditionFixture()
    {
        auto& components = this->world.components();
        components.registerComponent<HealthComponent>();
        components.registerComponent<VelocityComponent>();
    }

    TriggerConditionContext makeContext()
    { return TriggerConditionContext { this->world, this->entity, this->state }; }

    void addHealth(int current, int max = 100)
    { this->world.components().add<HealthComponent>(this->entity, HealthComponent { current, max }); }

    void addVelocity(float vx, float vy)
    { this->world.components().add<VelocityComponent>(this->entity, VelocityComponent { vx, vy });}
};

TEST_CASE_METHOD(CompositeConditionFixture, "CompositeCondition combines conditions with AND and OR",
    "[unit][composite_condition]"
) {
    this->state.timeInState = 0.75f;
    this->addHealth(40, 100);

    std::vector<std::unique_ptr<ITriggerCondition>> andConditions;
    andConditions.push_back(std::make_unique<MinTimeCondition>(0.5f));
    andConditions.push_back(std::make_unique<HealthBelowCondition>(50));

    CompositeCondition andCondition(CompositeCondition::And, std::move(andConditions));
    REQUIRE(andCondition.evaluate(this->makeContext()));

    std::vector<std::unique_ptr<ITriggerCondition>> orConditions;
    orConditions.push_back(std::make_unique<HealthBelowCondition>(10));
    orConditions.push_back(std::make_unique<VelocityAboveCondition>(0.5f));

    CompositeCondition orCondition(CompositeCondition::Or, std::move(orConditions));
    REQUIRE_FALSE(orCondition.evaluate(this->makeContext()));

    this->addVelocity(0.6f, 0.f);
    REQUIRE(orCondition.evaluate(this->makeContext()));
}
