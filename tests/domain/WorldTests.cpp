#include "../../src/domain/include/World/World.h"

#include <catch2/catch_test_macros.hpp>

struct Position { float x, y = 0.0f; };
struct Velocity { float dx, dy = 0.0f; };
struct Health { int hp = 100; };

TEST_CASE("World can create entities",
    "[integration][world]"
) {
    World world;
    auto& entities = world.entities();

    Entity e1 = entities.create();
    Entity e2 = entities.create();

    REQUIRE(e1.id != e2.id);
}

TEST_CASE("World can add components to entities",
    "[integration][world]"
) {
    World world;
    auto& components = world.components();

    components.registerComponent<Position>();

    Entity e = world.entities().create();

    components.add<Position>(e, Position{1.0f, 2.0f});

    REQUIRE(components.has<Position>(e));
}

TEST_CASE("World can get components from entities",
    "[integration][world]"
) {
    World world;
    auto& components = world.components();

    components.registerComponent<Position>();

    Entity e = world.entities().create();

    components.add(e, Position{1.0f, 2.0f});

    Position& pos = components.get<Position>(e);

    REQUIRE(pos.x == 1.0f);
    REQUIRE(pos.y == 2.0f);
}

TEST_CASE("World can remove components from entities",
    "[integration][world]"
) {
    World world;
    auto& components = world.components();

    components.registerComponent<Position>();

    Entity e = world.entities().create();

    components.add<Position>(e, Position{1.0f, 2.0f});
    REQUIRE(components.has<Position>(e));

    components.remove<Position>(e);

    REQUIRE_FALSE(components.has<Position>(e));
}

TEST_CASE("World supports multiple components per entity",
    "[integration][world]"
) {
    World world;
    auto& components = world.components();

    components.registerComponent<Position>();
    components.registerComponent<Velocity>();

    Entity e = world.entities().create();

    components.add<Position>(e, Position{1.0f, 2.0f});
    components.add<Velocity>(e, Velocity{0.5f, 0.5f});

    REQUIRE(components.has<Position>(e));
    REQUIRE(components.has<Velocity>(e));
}

TEST_CASE("World must allow modifying components in place",
    "[integration][world]"
) {
    World world;
    auto& components = world.components();

    components.registerComponent<Position>();

    Entity e = world.entities().create();

    components.add<Position>(e, Position{1.0f, 2.0f});

    Position& pos = components.get<Position>(e);

    pos.x = 3.0f;
    pos.y = 4.0f;

    REQUIRE(components.get<Position>(e).x == 3.0f);
    REQUIRE(components.get<Position>(e).y == 4.0f);
}

TEST_CASE("World view iterates over entities with specific components",
    "[integration][world]"
) {
    World world;
    auto& components = world.components();
    auto& entities = world.entities();

    components.registerComponent<Position>();
    components.registerComponent<Velocity>();

    Entity e1 = entities.create();
    Entity e2 = entities.create();
    Entity e3 = entities.create();

    components.add(e1, Position{1.0f, 2.0f});
    components.add(e1, Velocity{0.5f, 0.5f});

    components.add(e2, Position{3.0f, 4.0f});

    components.add(e3, Position{5.0f, 6.0f});
    components.add(e3, Velocity{1.0f, 1.0f});

    int count = 0;

    for (auto [entity, pos, vel] : View<Position, Velocity>(world.components()))
    {
        REQUIRE(components.has<Position>(entity));
        REQUIRE(components.has<Velocity>(entity));
        count++;
    }

    REQUIRE(count == 2);
}

TEST_CASE("World views return empty when no entities match",
    "[integration][world]"
) {
    World world;
    auto& components = world.components();

    components.registerComponent<Position>();
    components.registerComponent<Velocity>();

    Entity e = world.entities().create();

    components.add<Position>(e, Position{1.0f, 2.0f});

    int count = 0;

    for (auto _ : View<Position, Velocity>(world.components())) count++;

    REQUIRE(count == 0);
}
