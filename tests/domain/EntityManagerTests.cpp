#include "../../src/domain/include/EntityManager/EntityManager.h"

#include <catch2/catch_test_macros.hpp>

struct EntityManagerFixture
{
    EntityManager manager;
};

TEST_CASE_METHOD(
    EntityManagerFixture,
    "EntityManager creates unique entities",
    "[unit][entity_manager]")
{
    Entity e1 = manager.create();
    Entity e2 = manager.create();

    REQUIRE(e1 != e2);
}
