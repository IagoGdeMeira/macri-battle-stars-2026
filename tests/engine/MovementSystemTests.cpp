#include "../../src/engine/include/MovementSystem/MovementSystem.h"

#include "../../src/domain/components/TransformComponent.h"
#include "../../src/domain/components/VelocityComponent.h"

#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/Scene/Scene.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("MovementSystem updates position",
    "[integration][movement_system]"
) {
    EventBus bus;
    Scene scene(bus);

    auto e = scene.world().entities().create();

    scene.world().components().registerComponent<TransformComponent>();
    scene.world().components().registerComponent<VelocityComponent>();

    scene.world().components().add<TransformComponent>(e, TransformComponent{0.f, 0.f});
    scene.world().components().add<VelocityComponent>(e, VelocityComponent{1.f, 2.f});

    scene.systems().addSystem<MovementSystem>();

    scene.update(1.0f);

    auto& pos = scene.world().components().get<TransformComponent>(e);

    REQUIRE(pos.x == 1.f);
    REQUIRE(pos.y == 2.f);
}

TEST_CASE("MovementSystem does not update position without velocity",
    "[integration][movement_system]"
) {
    EventBus bus;
    Scene scene(bus);

    auto e = scene.world().entities().create();

    scene.world().components().registerComponent<TransformComponent>();
    scene.world().components().registerComponent<VelocityComponent>();

    scene.world().components().add<TransformComponent>(e, TransformComponent{0.f, 0.f});

    scene.systems().addSystem<MovementSystem>();

    scene.update(1.0f);

    auto& pos = scene.world().components().get<TransformComponent>(e);

    REQUIRE(pos.x == 0.f);
    REQUIRE(pos.y == 0.f);
}
