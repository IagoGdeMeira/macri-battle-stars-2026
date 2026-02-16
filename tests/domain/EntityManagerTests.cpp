#include "../../src/domain/include/EntityManager/EntityManager.h"

#include <catch2/catch_test_macros.hpp>

struct EntityManagerFixture { EntityManager manager; };

TEST_CASE_METHOD(
    EntityManagerFixture,
    "EntityManager creates unique alive entities",
    "[unit][entity_manager]"
) {
    Entity e1 = this->manager.create();
    Entity e2 = this->manager.create();

    REQUIRE(e1 != e2);
    REQUIRE(this->manager.isAlive(e1));
    REQUIRE(this->manager.isAlive(e2));
}

TEST_CASE_METHOD(
    EntityManagerFixture,
    "EntityManager::destroy makes entity not alive",
    "[unit][entity_manager]"
) {
    Entity e = this->manager.create();

    REQUIRE(this->manager.isAlive(e));

    this->manager.destroy(e);

    REQUIRE_FALSE(this->manager.isAlive(e));
}

TEST_CASE_METHOD(
    EntityManagerFixture,
    "EntityManager reuses id after destroy",
    "[unit][entity_manager]"
) {
    Entity e1 = this->manager.create();
    size_t reusedId = e1.index();

    this->manager.destroy(e1);

    Entity e2 = this->manager.create();

    REQUIRE(e2.index() == reusedId);
}

TEST_CASE_METHOD(
    EntityManagerFixture,
    "EntityManager increments generation when reusing id",
    "[unit][entity_manager]"
) {
    Entity e1 = this->manager.create();
    size_t id = e1.index();
    size_t gen = e1.gen();

    this->manager.destroy(e1);

    Entity e2 = this->manager.create();

    REQUIRE(e2.index() == id);
    REQUIRE(e2.gen() == gen + 1);
}

TEST_CASE_METHOD(
    EntityManagerFixture,
    "Old entity handle remains invalid after id reuse",
    "[unit][entity_manager]"
) {
    Entity e1 = this->manager.create();
    this->manager.destroy(e1);

    Entity e2 = this->manager.create();

    REQUIRE_FALSE(this->manager.isAlive(e1));
    REQUIRE(this->manager.isAlive(e2));
}

TEST_CASE_METHOD(
    EntityManagerFixture,
    "Destroying entity twice does not corrupt manager",
    "[unit][entity_manager]"
) {
    Entity e = this->manager.create();

    this->manager.destroy(e);
    this->manager.destroy(e);

    REQUIRE_FALSE(this->manager.isAlive(e));

    Entity newEntity = this->manager.create();

    REQUIRE(this->manager.isAlive(newEntity));
}

TEST_CASE_METHOD(
    EntityManagerFixture,
    "isAlive returns false for out of range entity",
    "[unit][entity_manager]"
) {
    Entity fake{9999, 0};

    REQUIRE_FALSE(this->manager.isAlive(fake));
}
