#include "../../src/domain/include/ComponentManager/ComponentManager.h"

#include "../../src/domain/include/Entity/Entity.h"

#include <catch2/catch_test_macros.hpp>

struct Position { float x, y = 0.0f; };
struct Velocity { float dx, dy = 0.0f; };

TEST_CASE("ComponentManager can register components",
    "[unit][component_manager]"
) {
    ComponentManager manager;
    
    manager.registerComponent<Position>();
    manager.registerComponent<Velocity>();

    SUCCEED();
}

TEST_CASE("ComponentManager can add and get components",
    "[unit][component_manager]"
) {
    ComponentManager manager;

    manager.registerComponent<Position>();

    Entity e{1};
    manager.add(e, Position{10.0f, 20.0f});

    auto& p = manager.get<Position>(e);

    REQUIRE(p.x == 10.0f);
    REQUIRE(p.y == 20.0f);
}

TEST_CASE("ComponentManager can check for components",
    "[unit][component_manager]"
) {
    ComponentManager manager;

    manager.registerComponent<Position>();

    Entity e{1};
    manager.add(e, Position{10.0f, 20.0f});

    REQUIRE(manager.has<Position>(e));
}

TEST_CASE("ComponentManager can remove components",
    "[unit][component_manager]"
) {
    ComponentManager manager;

    manager.registerComponent<Position>();

    Entity e{1};
    manager.add(e, Position{10.0f, 20.0f});

    manager.remove<Position>(e);

    REQUIRE_FALSE(manager.has<Position>(e));
}

TEST_CASE("ComponentManager can handle entity destruction",
    "[unit][component_manager]"
) {
    ComponentManager manager;

    manager.registerComponent<Position>();

    Entity e{1};
    manager.add(e, Position{10.0f, 20.0f});

    manager.entityDestroyed(e);

    REQUIRE_FALSE(manager.has<Position>(e));
}
