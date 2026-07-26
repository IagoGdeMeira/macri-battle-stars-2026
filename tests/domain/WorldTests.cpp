#include "domain/include/World/World.h"

#include <catch2/catch_test_macros.hpp>

struct Position { float x = 0.f, y = 0.f; };
struct Velocity { float dx = 0.f, dy = 0.f; };
struct Health { int hp = 100; };

TEST_CASE("World can create entities", "[integration][world]")
{
    World world;
    auto& entities = world.entities();

    Entity e1 = entities.create();
    Entity e2 = entities.create();

    REQUIRE(e1.id != e2.id);
}

TEST_CASE("World can add components to entities", "[integration][world]")
{
    World world;
    auto& comp = world.components();

    comp.registerComponent<Position>();
    Entity e = world.entities().create();

    comp.add<Position>(e, Position{1.f, 2.f});

    REQUIRE(comp.has<Position>(e));
}

TEST_CASE("World can get components from entities", "[integration][world]")
{
    World world;
    auto& comp = world.components();

    comp.registerComponent<Position>();

    Entity e = world.entities().create();

    comp.add(e, Position{1.f, 2.f});

    Position& pos = comp.get<Position>(e);

    REQUIRE(pos.x == 1.f);
    REQUIRE(pos.y == 2.f);
}

TEST_CASE("World can remove components from entities", "[integration][world]")
{
    World world;
    auto& comp = world.components();

    comp.registerComponent<Position>();

    Entity e = world.entities().create();

    comp.add<Position>(e, Position{1.f, 2.f});
    REQUIRE(comp.has<Position>(e));

    comp.remove<Position>(e);

    REQUIRE_FALSE(comp.has<Position>(e));
}

TEST_CASE("World supports multiple components per entity", "[integration][world]")
{
    World world;
    auto& comp = world.components();

    comp.registerComponent<Position>();
    comp.registerComponent<Velocity>();

    Entity e = world.entities().create();

    comp.add<Position>(e, Position{1.f, 2.f});
    comp.add<Velocity>(e, Velocity{0.5f, 0.5f});

    REQUIRE(comp.has<Position>(e));
    REQUIRE(comp.has<Velocity>(e));
}

TEST_CASE("World must allow modifying components in place", "[integration][world]")
{
    World world;
    auto& comp = world.components();

    comp.registerComponent<Position>();

    Entity e = world.entities().create();

    comp.add<Position>(e, Position{1.f, 2.f});

    Position& pos = comp.get<Position>(e);

    pos.x = 3.f;
    pos.y = 4.f;

    REQUIRE(comp.get<Position>(e).x == 3.f);
    REQUIRE(comp.get<Position>(e).y == 4.f);
}

TEST_CASE("World view iterates over entities with specific components", "[integration][world]")
{
    World world;
    auto& comp = world.components();
    auto& entities = world.entities();

    comp.registerComponent<Position>();
    comp.registerComponent<Velocity>();

    Entity e1 = entities.create();
    Entity e2 = entities.create();
    Entity e3 = entities.create();


    comp.add(e1, Position{1.f, 2.f});
    comp.add(e1, Velocity{0.5f, 0.5f});

    comp.add(e2, Position{3.f, 4.f});

    comp.add(e3, Position{5.f, 6.f});
    comp.add(e3, Velocity{1.f, 1.f});

    int count = 0;

    for (auto [entity, pos, vel] : View<Position, Velocity>(comp))
    {
        REQUIRE(comp.has<Position>(entity));
        REQUIRE(comp.has<Velocity>(entity));
        count++;
    }

    REQUIRE(count == 2);
}

TEST_CASE("World views return empty when no entities match", "[integration][world]")
{
    World world;
    auto& comp = world.components();

    comp.registerComponent<Position>();
    comp.registerComponent<Velocity>();

    Entity e = world.entities().create();

    comp.add<Position>(e, Position{1.f, 2.f});

    int count = 0;
    for (auto &&item : View<Position, Velocity>(comp)) (void)item, count++;

    REQUIRE(count == 0);
}
