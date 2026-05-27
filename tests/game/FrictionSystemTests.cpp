#include "../../src/game/include/FrictionSystem/FrictionSystem.h"

#include "../../src/domain/components/GroundedComponent.h"
#include "../../src/domain/components/HitstopComponent.h"
#include "../../src/domain/components/VelocityComponent.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class FrictionSystemFixture
{
public:
    FrictionSystemFixture() : system(10.0f), context { world, bus, commandBuffer, 0.0f }
    {
        auto& components = this->world.components();
        components.registerComponent<GroundedComponent>();
        components.registerComponent<HitstopComponent>();
        components.registerComponent<VelocityComponent>();
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    FrictionSystem system;
    UpdateContext context;
};

TEST_CASE_METHOD(FrictionSystemFixture, "FrictionSystem reduces horizontal velocity when entity is grounded",
    "[unit][friction_system]"
) {
    auto& components = this->world.components();
    const auto entity = this->world.entities().create();

    components.add<VelocityComponent>(entity, VelocityComponent{ 10.0f, 0.0f });
    components.add<GroundedComponent>(entity, GroundedComponent{ true, 0.0f });

    this->context.deltaTime = 1.0f;
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == Catch::Approx(9.0f));
    REQUIRE(v.velocity.y == 0.0f);
}

TEST_CASE_METHOD(FrictionSystemFixture, "FrictionSystem does not affect entities that are not grounded",
    "[unit][friction_system]"
) {
    auto& components = this->world.components();
    const auto entity = this->world.entities().create();

    components.add<VelocityComponent>(entity, VelocityComponent{ 12.0f, -3.0f });
    components.add<GroundedComponent>(entity, GroundedComponent{ false, 0.0f });

    this->context.deltaTime = 1.0f;
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == 12.0f);
    REQUIRE(v.velocity.y == -3.0f);
}

TEST_CASE_METHOD(FrictionSystemFixture, "FrictionSystem applies friction reduction from GroundedComponent",
    "[unit][friction_system]"
) {
    auto& components = this->world.components();
    const auto entity = this->world.entities().create();

    components.add<VelocityComponent>(entity, VelocityComponent{ 10.0f, 0.0f });
    components.add<GroundedComponent>(entity, GroundedComponent{ true, 0.5f });

    this->context.deltaTime = 1.0f;
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == Catch::Approx(9.5f));
}

TEST_CASE_METHOD(FrictionSystemFixture, "FrictionSystem clamps friction reduction and snaps small velocity to zero",
    "[unit][friction_system]"
) {
    auto& components = this->world.components();
    const auto entity = this->world.entities().create();

    components.add<VelocityComponent>(entity, VelocityComponent{ 0.8f, 0.0f });
    components.add<GroundedComponent>(entity, GroundedComponent{ true, 2.0f });

    this->context.deltaTime = 1.0f;
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == 0.0f);
}

TEST_CASE_METHOD(FrictionSystemFixture, "FrictionSystem handles frictionReduction above one by clamping effective friction to zero",
    "[unit][friction_system]"
) {
    auto& components = this->world.components();
    const auto entity = this->world.entities().create();

    components.add<VelocityComponent>(entity, VelocityComponent{ 10.0f, 0.0f });
    components.add<GroundedComponent>(entity, GroundedComponent{ true, 2.0f });

    this->context.deltaTime = 1.0f;
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == 10.0f);
}

TEST_CASE_METHOD(FrictionSystemFixture, "FrictionSystem allows stronger friction when frictionReduction is negative",
    "[unit][friction_system]"
) {
    auto& components = this->world.components();
    const auto entity = this->world.entities().create();

    components.add<VelocityComponent>(entity, VelocityComponent{ 10.0f, 0.0f });
    components.add<GroundedComponent>(entity, GroundedComponent{ true, -0.5f });

    this->context.deltaTime = 1.0f;
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == Catch::Approx(8.5f));
}

TEST_CASE_METHOD(FrictionSystemFixture, "FrictionSystem ignores entities frozen by hitstop",
    "[integration][friction_system]"
) {
    auto& components = this->world.components();
    const auto entity = this->world.entities().create();

    components.add<VelocityComponent>(entity, VelocityComponent{ 10.0f, 0.0f });
    components.add<GroundedComponent>(entity, GroundedComponent{ true, 0.0f });
    components.add<HitstopComponent>(entity, HitstopComponent{ .remaining = 1.0f, .frozen = true });

    this->context.deltaTime = 1.0f;
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == Catch::Approx(10.0f));
}
