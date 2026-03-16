#include "../../src/domain/include/EntityManager/EntityManager.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("EntityManager can create entities",
    "[unit][entity_manager]"
) {
    EntityManager manager;
    Entity e = manager.create();

    REQUIRE(manager.isAlive(e));
}

TEST_CASE("EntityManager can assign unique IDs",
    "[unit][entity_manager]"
) {
    EntityManager manager;

    Entity e1 = manager.create();
    Entity e2 = manager.create();
    Entity e3 = manager.create();
    
    REQUIRE(e1.id != e2.id);
    REQUIRE(e2.id != e3.id);
    REQUIRE(e1.id != e3.id);
}

TEST_CASE("EntityManager can destroy entities",
    "[unit][entity_manager]"
) {
    EntityManager manager;
    Entity e = manager.create();

    manager.destroy(e);

    REQUIRE_FALSE(manager.isAlive(e));
}

TEST_CASE("EntityManager can reuse IDs of destroyed entities",
    "[unit][entity_manager]"
) {
    EntityManager manager;

    Entity e1 = manager.create();
    uint32_t id = e1.id;

    manager.destroy(e1);
    Entity e2 = manager.create();

    REQUIRE(e2.id == id);
}

TEST_CASE("EntityManager can destroy an entity twice without crashing",
    "[unit][entity_manager]"
) {
    EntityManager manager;

    Entity e = manager.create();

    manager.destroy(e);
    manager.destroy(e);

    REQUIRE_FALSE(manager.isAlive(e));
}

TEST_CASE("Invalid entities are not alive",
    "[unit][entity_manager]"
) {
    EntityManager manager;

    Entity invalidEntity(9999); // Assuming 9999 is an ID that was never created

    REQUIRE_FALSE(manager.isAlive(invalidEntity));
}

TEST_CASE("EntityManager can handle multiple entities' lifecycles",
    "[unit][entity_manager]"
) {
    EntityManager manager;

    Entity e1 = manager.create();
    Entity e2 = manager.create();
    Entity e3 = manager.create();

    manager.destroy(e2);

    REQUIRE(manager.isAlive(e1));
    REQUIRE_FALSE(manager.isAlive(e2));
    REQUIRE(manager.isAlive(e3));
}

TEST_CASE("EntityManager can handle a large number of entities",
    "[unit][entity_manager]"
) {
    EntityManager manager;

    const int numEntities = 10000;
    std::vector<Entity> entities;

    for (int i = 0; i < numEntities; ++i) entities.push_back(manager.create());

    for (const auto& e : entities) REQUIRE(manager.isAlive(e));
}
