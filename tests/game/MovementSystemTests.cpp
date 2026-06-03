#include "../../src/game/include/MovementSystem/MovementSystem.h"

#include "../../src/domain/components/HitstopComponent.h"
#include "../../src/domain/components/TransformComponent.h"
#include "../../src/domain/components/VelocityComponent.h"

#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/Scene/Scene.h"

#include <catch2/catch_test_macros.hpp>

class MovementSystemFixture
{
public:
    MovementSystemFixture() : bus(), scene(bus)
    {
        auto& components = this->scene.world().components();
        components.registerComponent<TransformComponent>();
        components.registerComponent<VelocityComponent>();
        components.registerComponent<HitstopComponent>();

        this->scene.systems().addSystem<MovementSystem>();
    }

    Entity createMovingEntity(float x, float y, float vx, float vy)
    {
        auto entity = this->scene.world().entities().create();
        auto& components = this->scene.world().components();

        components.add<TransformComponent>(entity, TransformComponent{x, y});
        components.add<VelocityComponent>(entity, VelocityComponent{vx, vy});

        return entity;
    }

    Entity createStaticEntity(float x, float y)
    {
        auto entity = this->scene.world().entities().create();
        auto& components = this->scene.world().components();

        components.add<TransformComponent>(entity, TransformComponent{x, y});

        return entity;
    }

    Entity createFrozenEntity(float x, float y, float vx, float vy)
    {
        auto entity = createMovingEntity(x, y, vx, vy);
        this->scene.world().components().add<HitstopComponent>(entity, 
            HitstopComponent{ .remaining = 1.0f, .frozen = true });

        return entity;
    }

protected:
    EventBus bus;
    Scene scene;
};

TEST_CASE_METHOD(MovementSystemFixture, "MovementSystem updates position",
    "[integration][movement_system]"
) {
    auto e = this->createMovingEntity(0.f, 0.f, 1.f, 2.f);

    this->scene.update(1.0f);

    auto& pos = this->scene.world().components().get<TransformComponent>(e);

    REQUIRE(pos.position.x == 1.f);
    REQUIRE(pos.position.y == 2.f);
}

TEST_CASE_METHOD(MovementSystemFixture, "MovementSystem does not update position without velocity",
    "[integration][movement_system]"
) {
    auto e = this->createStaticEntity(0.f, 0.f);

    this->scene.update(1.0f);

    auto& pos = this->scene.world().components().get<TransformComponent>(e);

    REQUIRE(pos.position.x == 0.f);
    REQUIRE(pos.position.y == 0.f);
}

TEST_CASE_METHOD(MovementSystemFixture, "MovementSystem does not update position when entity is frozen by hitstop",
    "[integration][movement_system]"
) {
    auto e = this->createFrozenEntity(0.f, 0.f, 10.f, 20.f);

    this->scene.update(1.0f);

    auto& pos = this->scene.world().components().get<TransformComponent>(e);

    REQUIRE(pos.position.x == 0.f);
    REQUIRE(pos.position.y == 0.f);
}
