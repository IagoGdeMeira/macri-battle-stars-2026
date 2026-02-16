#include "../../src/domain/include/EntityManager/EntityManager.h"

#include <catch2/catch_test_macros.hpp>

struct EntityManagerFixture { EntityManager manager; };

TEST_CASE_METHOD(
    EntityManagerFixture,
    "EntityManager creates unique alive entities",
    "[unit][entity_manager]"
) {
    Entity e1 = manager.create();
    Entity e2 = manager.create();

    REQUIRE(e1 != e2);
    REQUIRE(manager.isAlive(e1));
    REQUIRE(manager.isAlive(e2));
}

TEST_CASE_METHOD(
    EntityManagerFixture,
    "EntityManager::destroy makes entity not alive",
    "[unit][entity_manager]")
{
    Entity e = manager.create();

    REQUIRE(manager.isAlive(e));

    manager.destroy(e);

    REQUIRE_FALSE(manager.isAlive(e));
}

TEST_CASE_METHOD(
    EntityManagerFixture,
    "EntityManager reuses id after destroy",
    "[unit][entity_manager]")
{
    Entity e1 = manager.create();
    size_t reusedId = e1.index();

    manager.destroy(e1);

    Entity e2 = manager.create();

    REQUIRE(e2.index() == reusedId);
}

TEST_CASE_METHOD(
    EntityManagerFixture,
    "EntityManager increments generation when reusing id",
    "[unit][entity_manager]")
{
    Entity e1 = manager.create();
    size_t id = e1.index();
    size_t gen = e1.gen();

    manager.destroy(e1);

    Entity e2 = manager.create();

    REQUIRE(e2.index() == id);
    REQUIRE(e2.gen() == gen + 1);
}

TEST_CASE_METHOD(
    EntityManagerFixture,
    "Old entity handle remains invalid after id reuse",
    "[unit][entity_manager]")
{
    Entity e1 = manager.create();
    manager.destroy(e1);

    Entity e2 = manager.create();

    REQUIRE_FALSE(manager.isAlive(e1));
    REQUIRE(manager.isAlive(e2));
}

TEST_CASE_METHOD(
    EntityManagerFixture,
    "Destroying entity twice does not corrupt manager",
    "[unit][entity_manager]")
{
    Entity e = manager.create();

    manager.destroy(e);
    manager.destroy(e);

    REQUIRE_FALSE(manager.isAlive(e));

    Entity newEntity = manager.create();

    REQUIRE(manager.isAlive(newEntity));
}

TEST_CASE_METHOD(
    EntityManagerFixture,
    "isAlive returns false for out of range entity",
    "[unit][entity_manager]")
{
    Entity fake{9999, 0};

    REQUIRE_FALSE(manager.isAlive(fake));
}
