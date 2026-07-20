#include "game/trigger_conditions/NotCondition.h"

#include "domain/components/HealthComponent.h"
#include "domain/components/StateComponent.h"
#include "domain/include/World/World.h"

#include "game/trigger_conditions/HealthBelowCondition.h"

#include <catch2/catch_test_macros.hpp>

class NotConditionFixture
{
public:
    World world;
    StateComponent state;
    Entity entity{1};
    
    NotConditionFixture()
    { this->world.components().registerComponent<HealthComponent>(); }

    TriggerConditionContext makeContext()
    { return TriggerConditionContext { this->world, this->entity, this->state }; }

    void addHealth(int current, int max = 100)
    { this->world.components().add<HealthComponent>(this->entity, HealthComponent { current, max }); }
};

TEST_CASE_METHOD(NotConditionFixture, "NotCondition inverts the nested condition result",
    "[unit][conditions][not]"
) {
    this->addHealth(80, 100);

    NotCondition condition(std::make_unique<HealthBelowCondition>(50));

    REQUIRE(condition.evaluate(this->makeContext()));
}
