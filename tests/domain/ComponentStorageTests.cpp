#include "../../src/domain/include/ComponentStorage/ComponentStorage.h"

#include <catch2/catch_test_macros.hpp>

struct Position { float x, y; };

struct ComponentStorageFixture { ComponentStorage<Position> storage; };

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "ComponentStorage adds and retrieves component",
    "[unit][component_storage]"
) {
    Entity e{0, 0};
    this->storage.add(e, {10, 20});

    REQUIRE(this->storage.has(e));
    REQUIRE(this->storage.size() == 1);

    Position &p = this->storage.get(e);
    REQUIRE(p.x == 10);
    REQUIRE(p.y == 20);
}

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "ComponentStorage get throws if component not found",
    "[unit][component_storage]"
) {
    Entity e{1, 0};

    REQUIRE_FALSE(this->storage.has(e));
    REQUIRE_THROWS(this->storage.get(e));
}

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "ComponentStorage throws when adding duplicate component",
    "[unit][component_storage]"
) {
    Entity e{2, 0};

    this->storage.add(e, {1, 1});

    REQUIRE_THROWS(this->storage.add(e, {2, 2}));
    REQUIRE(this->storage.size() == 1);
}

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "ComponentStorage removes component correctly",
    "[unit][component_storage]"
) {
    Entity e{3, 0};

    this->storage.add(e, {5, 6});
    REQUIRE(this->storage.size() == 1);

    this->storage.remove(e);

    REQUIRE(this->storage.size() == 0);
    REQUIRE_FALSE(this->storage.has(e));
}

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "Removing non-existent component does nothing",
    "[unit][component_storage]"
) {
    Entity e{4, 0};

    this->storage.remove(e);

    REQUIRE(this->storage.size() == 0);
}

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "ComponentStorage keeps dense array compact after removal",
    "[unit][component_storage]"
) {
    Entity e1{0, 0};
    Entity e2{1, 0};
    Entity e3{2, 0};

    this->storage.add(e1, {1, 1});
    this->storage.add(e2, {2, 2});
    this->storage.add(e3, {3, 3});

    REQUIRE(this->storage.size() == 3);

    this->storage.remove(e2);

    REQUIRE(this->storage.size() == 2);

    REQUIRE(this->storage.has(e1));
    REQUIRE_FALSE(this->storage.has(e2));
    REQUIRE(this->storage.has(e3));

    REQUIRE(this->storage.getEntities().size() == this->storage.getComponents().size());
}

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "ComponentStorage distinguishes entities by generation",
    "[unit][component_storage]"
) {
    Entity e1{5, 0};
    Entity e2{5, 1};

    this->storage.add(e1, {9, 9});

    REQUIRE(this->storage.has(e1));
    REQUIRE_FALSE(this->storage.has(e2));
}

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "ComponentStorage allows modifying component through get",
    "[unit][component_storage]"
) {
    Entity e{6, 0};

    this->storage.add(e, {7, 7});
    this->storage.get(e).x = 42;

    REQUIRE(this->storage.get(e).x == 42);
}
