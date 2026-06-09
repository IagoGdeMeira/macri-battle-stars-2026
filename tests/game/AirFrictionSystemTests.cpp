#include "../../src/game/include/AirFrictionSystem/AirFrictionSystem.h"

#include "../../src/domain/components/AirFrictionComponent.h"
#include "../../src/domain/components/GroundedComponent.h"
#include "../../src/domain/components/HitstopComponent.h"
#include "../../src/domain/components/VelocityComponent.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class AirFrictionSystemFixture
{
public:
    AirFrictionSystemFixture() : system(10.0f), context { this->world, this->bus, this->commandBuffer, 0.0f }
    {
        auto& components = this->world.components();
        components.registerComponent<AirFrictionComponent>();
        components.registerComponent<GroundedComponent>();
        components.registerComponent<HitstopComponent>();
        components.registerComponent<VelocityComponent>();
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    AirFrictionSystem system;
    UpdateContext context;
};

TEST_CASE_METHOD(AirFrictionSystemFixture,
    "AirFrictionSystem reduces velocity on both axes when entity is airborne",
    "[unit][air_friction_system]"
) {
    auto& components = this->world.components();
    const auto entity = this->world.entities().create();

    components.add<VelocityComponent>(entity, VelocityComponent{ 10.0f, -20.0f });
    components.add<GroundedComponent>(entity, GroundedComponent{ false, 0.0f });
    components.add<AirFrictionComponent>(entity, AirFrictionComponent{ 1.0f, 1.0f });

    this->context.deltaTime = 1.0f;
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == Catch::Approx(9.0f));
    REQUIRE(v.velocity.y == Catch::Approx(-18.0f));
}

TEST_CASE_METHOD(AirFrictionSystemFixture,
    "AirFrictionSystem does not affect grounded entities",
    "[unit][air_friction_system]"
) {
    auto& components = this->world.components();
    const auto entity = this->world.entities().create();

    components.add<VelocityComponent>(entity, VelocityComponent{ 12.0f, -8.0f });
    components.add<GroundedComponent>(entity, GroundedComponent{ true, 0.0f });
    components.add<AirFrictionComponent>(entity, AirFrictionComponent{ 2.0f, 2.0f });

    this->context.deltaTime = 1.0f;
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == 12.0f);
    REQUIRE(v.velocity.y == -8.0f);
}

TEST_CASE_METHOD(AirFrictionSystemFixture,
    "AirFrictionSystem applies per-axis multipliers from AirFrictionComponent",
    "[unit][air_friction_system]"
) {
    auto& components = this->world.components();
    const auto entity = this->world.entities().create();

    components.add<VelocityComponent>(entity, VelocityComponent{ 10.0f, 10.0f });
    components.add<GroundedComponent>(entity, GroundedComponent{ false, 0.0f });
    components.add<AirFrictionComponent>(entity, AirFrictionComponent{ 0.5f, 2.0f });

    this->context.deltaTime = 1.0f;
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == Catch::Approx(9.5f));
    REQUIRE(v.velocity.y == Catch::Approx(8.0f));
}

TEST_CASE_METHOD(AirFrictionSystemFixture,
    "AirFrictionSystem clamps negative multipliers and snaps small velocity to zero",
    "[unit][air_friction_system]"
) {
    auto& components = this->world.components();
    const auto entity = this->world.entities().create();

    components.add<VelocityComponent>(entity, VelocityComponent{ 0.8f, -0.7f });
    components.add<GroundedComponent>(entity, GroundedComponent{ false, 0.0f });
    components.add<AirFrictionComponent>(entity, AirFrictionComponent{ -5.0f, -1.0f });

    this->context.deltaTime = 1.0f;
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == 0.0f);
    REQUIRE(v.velocity.y == 0.0f);
}

TEST_CASE_METHOD(AirFrictionSystemFixture,
    "AirFrictionSystem clamps decay and can stop velocity when friction is very high",
    "[unit][air_friction_system]"
) {
    auto& components = this->world.components();
    const auto entity = this->world.entities().create();

    components.add<VelocityComponent>(entity, VelocityComponent{ 20.0f, -20.0f });
    components.add<GroundedComponent>(entity, GroundedComponent{ false, 0.0f });
    components.add<AirFrictionComponent>(entity, AirFrictionComponent{ 20.0f, 20.0f });

    this->context.deltaTime = 1.0f;
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == 0.0f);
    REQUIRE(v.velocity.y == 0.0f);
}
