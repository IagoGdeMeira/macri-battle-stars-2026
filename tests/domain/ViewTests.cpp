#include "domain/include/View/View.h"

#include "domain/include/ComponentManager/ComponentManager.h"
#include "domain/include/ComponentStorage/ComponentStorage.h"
#include "domain/include/Entity/Entity.h"

#include <catch2/catch_test_macros.hpp>

struct Position { float x = 0.f, y = 0.f; };
struct Velocity { float dx = 0.f, dy = 0.f; };
struct Health { int hp = 100; };

TEST_CASE("View can only iterate over entities with specific components", "[unit][view]")
{
    ComponentManager manager;

    manager.registerComponent<Position>();
    manager.registerComponent<Velocity>();

    Entity e1(0);
    Entity e2(1);
    Entity e3(2);

    manager.add<Position>(e1, Position{1.f, 2.f});
    manager.add<Velocity>(e1, Velocity{0.5f, 0.5f});

    manager.add<Position>(e2, Position{3.f, 4.f});

    manager.add<Position>(e3, Position{5.f, 6.f});
    manager.add<Velocity>(e3, Velocity{1.f, 1.f});

    View<Position, Velocity> view(manager);

    int count = 0;

    for (auto [entity, pos, vel] : view)
    {
        REQUIRE(manager.has<Position>(entity));
        REQUIRE(manager.has<Velocity>(entity));
        count++;
    }

    REQUIRE(count == 2);
}

TEST_CASE("View can support three component queries", "[unit][view]")
{
    ComponentManager manager;

    manager.registerComponent<Position>();
    manager.registerComponent<Velocity>();
    manager.registerComponent<Health>();

    Entity e1(0);
    Entity e2(1);

    manager.add<Position>(e1, Position{1.f, 2.f});
    manager.add<Velocity>(e1, Velocity{0.5f, 0.5f});
    manager.add<Health>(e1, Health{ 80 });

    manager.add<Position>(e2, Position{3.f, 4.f});
    manager.add<Velocity>(e2, Velocity{1.f, 1.f});

    View<Position, Velocity, Health> view(manager);

    int count = 0;

    for (auto [entity, pos, vel, health] : view)
    {
        REQUIRE(entity.id == 0);
        REQUIRE(health.hp == 80);
        count++;
    }

    REQUIRE(count == 1);
}

TEST_CASE("View must return empty if no entities match the query", "[unit][view]")
{
    ComponentManager manager;

    manager.registerComponent<Position>();
    manager.registerComponent<Velocity>();

    Entity e1(0);

    manager.add<Position>(e1, Position{1.f, 2.f});

    View<Position, Velocity> view(manager);

    int count = 0;

    for (auto _ : view) count++;
    REQUIRE(count == 0);
}

TEST_CASE("View must iterate over all matching entities",
    "[unit][view]"
) {
    ComponentManager manager;

    manager.registerComponent<Position>();
    manager.registerComponent<Velocity>();

    for (uint32_t i = 0; i < 5; i++)
    {
        Entity e(i);
        manager.add<Position>(e, Position{});
        manager.add<Velocity>(e, Velocity{});
    }

    View<Position, Velocity> view(manager);

    int count = 0;
    for (auto [entity, pos, vel] : view) count++;

    REQUIRE(count == 5);
}
