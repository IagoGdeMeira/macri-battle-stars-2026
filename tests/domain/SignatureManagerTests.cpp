#include "../../src/domain/include/SignatureManager/SignatureManager.h"

#include <catch2/catch_test_macros.hpp>

struct Position { float x, y; };
struct Velocity { float dx, dy; };
struct Health { int hp; };

struct SignatureManagerFixture { SignatureManager manager; };

TEST_CASE_METHOD(
    SignatureManagerFixture,
    "SignatureManager adds and detects component",
    "[unit][signature_manager]"
) {
    Entity e{0, 0};
    size_t pos = this->manager.getComponentIndex<Position>();

    this->manager.addComponent(e, pos);

    REQUIRE(this->manager.hasComponent(e, pos));
}

TEST_CASE_METHOD(
    SignatureManagerFixture,
    "SignatureManager removes component correctly",
    "[unit][signature_manager]"
) {
    Entity e{1, 0};
    size_t pos = this->manager.getComponentIndex<Position>();

    this->manager.addComponent(e, pos);
    REQUIRE(this->manager.hasComponent(e, pos));

    this->manager.removeComponent(e, pos);
    REQUIRE_FALSE(this->manager.hasComponent(e, pos));
}

TEST_CASE_METHOD(
    SignatureManagerFixture,
    "SignatureManager match using indices works correctly",
    "[unit][signature_manager]"
) {
    Entity e{2, 0};

    size_t pos = this->manager.getComponentIndex<Position>();
    size_t vel = this->manager.getComponentIndex<Velocity>();
    size_t hp = this->manager.getComponentIndex<Health>();

    this->manager.addComponent(e, pos);
    this->manager.addComponent(e, vel);

    REQUIRE(this->manager.match(e, pos, vel));
    REQUIRE_FALSE(this->manager.match(e, pos, vel, hp));
}

TEST_CASE_METHOD(
    SignatureManagerFixture,
    "SignatureManager matchTypes works correctly",
    "[unit][signature_manager]"
) {
    Entity e{3, 0};

    size_t pos = this->manager.getComponentIndex<Position>();
    size_t vel = this->manager.getComponentIndex<Velocity>();

    this->manager.addComponent(e, pos);
    this->manager.addComponent(e, vel);

    REQUIRE(this->manager.matchTypes<Position, Velocity>(e));
    REQUIRE_FALSE(this->manager.matchTypes<Position, Health>(e));
}

TEST_CASE_METHOD(
    SignatureManagerFixture,
    "SignatureManager clear resets all bits",
    "[unit][signature_manager]"
) {
    Entity e{4, 0};

    size_t pos = this->manager.getComponentIndex<Position>();
    size_t vel = this->manager.getComponentIndex<Velocity>();

    this->manager.addComponent(e, pos);
    this->manager.addComponent(e, vel);

    this->manager.clear(e);

    REQUIRE_FALSE(this->manager.hasComponent(e, pos));
    REQUIRE_FALSE(this->manager.hasComponent(e, vel));
    REQUIRE_FALSE(this->manager.matchTypes<Position, Velocity>(e));
}

TEST_CASE_METHOD(
    SignatureManagerFixture,
    "SignatureManager safely handles non-existent entity",
    "[unit][signature_manager]"
) {
    Entity e{10, 0};

    size_t pos = this->manager.getComponentIndex<Position>();

    REQUIRE_FALSE(this->manager.hasComponent(e, pos));
    REQUIRE_FALSE(this->manager.matchTypes<Position>(e));

    this->manager.removeComponent(e, pos);
}

TEST_CASE_METHOD(
    SignatureManagerFixture,
    "SignatureManager handles component indices beyond one block",
    "[unit][signature_manager]"
) {
    Entity e{5, 0};

    size_t highIndex = 130;

    this->manager.addComponent(e, highIndex);

    REQUIRE(this->manager.hasComponent(e, highIndex));
}
