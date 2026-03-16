#include "../../src/domain/include/ComponentStorage/ComponentStorage.h"

#include "../../src/domain/include/Entity/Entity.h"

#include <catch2/catch_test_macros.hpp>

struct Position { float x, y = 0.0f; };
struct Velocity { float dx, dy = 0.0f; };

TEST_CASE("ComponentStorage can add components",
    "[unit][component_storage]"
) {
    ComponentStorage<Position> storage;
    Entity e{1};

    Position p{10.0f, 20.0f};
    storage.add(e, p);

    REQUIRE(storage.has(e));

    auto& result = storage.get(e);

    REQUIRE(result.x == 10.0f);
    REQUIRE(result.y == 20.0f);
}

TEST_CASE("ComponentStorage can remove components",
    "[unit][component_storage]"
) {
    ComponentStorage<Position> storage;
    Entity e{1};

    Position p{10.0f, 20.0f};
    storage.add(e, p);

    storage.remove(e);

    REQUIRE_FALSE(storage.has(e));
}

TEST_CASE("ComponentStorage can maintain data after swap removal",
    "[unit][component_storage]"
) {
    ComponentStorage<Position> storage;

    Entity e1{0};
    Entity e2{1};

    storage.add(e1, Position{10.0f, 20.0f});
    storage.add(e2, Position{30.0f, 40.0f});

    storage.remove(e1);

    REQUIRE(storage.has(e2));

    auto& p = storage.get(e2);

    REQUIRE(p.x == 30.0f);
    REQUIRE(p.y == 40.0f);
}

TEST_CASE("Size of ComponentStorage must reflect number of components",
    "[unit][component_storage]"
) {
    ComponentStorage<Position> storage;

    storage.add(Entity{0}, Position{10.0f, 20.0f});
    storage.add(Entity{1}, Position{30.0f, 40.0f});

    REQUIRE(storage.size() == 2);

    storage.remove(Entity{0});

    REQUIRE(storage.size() == 1);
}

TEST_CASE("ComponentStorage can return list of entities",
    "[unit][component_storage]"
) {
    ComponentStorage<Position> storage;

    Entity e1{0};
    Entity e2{1};

    storage.add(e1, Position{10.0f, 20.0f});
    storage.add(e2, Position{30.0f, 40.0f});

    const auto& entities = storage.entities();

    REQUIRE(entities.size() == 2);
}
