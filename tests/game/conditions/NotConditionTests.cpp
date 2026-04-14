#include "../../../src/domain/components/HealthComponent.h"
#include "../../../src/domain/components/StateComponent.h"
#include "../../../src/domain/include/World/World.h"
#include "../../../src/game/conditions/HealthBelowCondition.h"
#include "../../../src/game/conditions/NotCondition.h"

#include <catch2/catch_test_macros.hpp>

class NotConditionFixture
{
public:
    NotConditionFixture()
    { this->world.components().registerComponent<HealthComponent>(); }

    ConditionContext makeContext()
    { return ConditionContext { this->world, this->entity, this->state }; }

    void addHealth(int current, int max = 100)
    {
        this->world.components().add<HealthComponent>(
            this->entity, HealthComponent { current, max });
    }

    World world;
    StateComponent state;
    Entity entity { 1 };
};

TEST_CASE_METHOD(NotConditionFixture, "NotCondition inverts the nested condition result",
    "[unit][conditions][not]"
) {
    this->addHealth(80, 100);

    NotCondition condition(std::make_unique<HealthBelowCondition>(50));

    REQUIRE(condition.evaluate(this->makeContext()));
}
