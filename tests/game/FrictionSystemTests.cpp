#include "game/include/FrictionSystem/FrictionSystem.h"

#include "domain/components/GroundedComponent.h"
#include "domain/components/HitstopComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class FrictionSystemFixture
{
public:
    FrictionSystemFixture() : system(10.f), context { this->world, this->bus, this->commandBuffer, 0.f }
    {
        auto& comp = this->world.components();
        comp.registerComponent<GroundedComponent>();
        comp.registerComponent<HitstopComponent>();
        comp.registerComponent<VelocityComponent>();
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
    auto& comp = this->world.components();
    const auto entity = this->world.entities().create();

    comp.add<VelocityComponent>(entity, VelocityComponent{ 10.f, 0.f });
    comp.add<GroundedComponent>(entity, GroundedComponent{ true, 0.f });

    this->context.deltaTime = 1.f;
    this->system.update(this->context);

    const auto& v = comp.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == Catch::Approx(9.f));
    REQUIRE(v.velocity.y == 0.f);
}

TEST_CASE_METHOD(FrictionSystemFixture, "FrictionSystem does not affect entities that are not grounded",
    "[unit][friction_system]"
) {
    auto& comp = this->world.components();
    const auto entity = this->world.entities().create();

    comp.add<VelocityComponent>(entity, VelocityComponent{ 12.f, -3.f });
    comp.add<GroundedComponent>(entity, GroundedComponent{ false, 0.f });

    this->context.deltaTime = 1.f;
    this->system.update(this->context);

    const auto& v = comp.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == 12.f);
    REQUIRE(v.velocity.y == -3.f);
}

TEST_CASE_METHOD(FrictionSystemFixture, "FrictionSystem applies friction reduction from GroundedComponent",
    "[unit][friction_system]"
) {
    auto& comp = this->world.components();
    const auto entity = this->world.entities().create();

    comp.add<VelocityComponent>(entity, VelocityComponent{ 10.f, 0.f });
    comp.add<GroundedComponent>(entity, GroundedComponent{ true, 0.5f });

    this->context.deltaTime = 1.f;
    this->system.update(this->context);

    const auto& v = comp.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == Catch::Approx(9.5f));
}

TEST_CASE_METHOD(FrictionSystemFixture, "FrictionSystem clamps friction reduction and snaps small velocity to zero",
    "[unit][friction_system]"
) {
    auto& comp = this->world.components();
    const auto entity = this->world.entities().create();

    comp.add<VelocityComponent>(entity, VelocityComponent{ 0.8f, 0.f });
    comp.add<GroundedComponent>(entity, GroundedComponent{ true, 2.f });

    this->context.deltaTime = 1.f;
    this->system.update(this->context);

    const auto& v = comp.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == 0.f);
}

TEST_CASE_METHOD(FrictionSystemFixture, "FrictionSystem handles frictionReduction above one by clamping effective friction to zero",
    "[unit][friction_system]"
) {
    auto& comp = this->world.components();
    const auto entity = this->world.entities().create();

    comp.add<VelocityComponent>(entity, VelocityComponent{ 10.f, 0.f });
    comp.add<GroundedComponent>(entity, GroundedComponent{ true, 2.f });

    this->context.deltaTime = 1.f;
    this->system.update(this->context);

    const auto& v = comp.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == 10.f);
}

TEST_CASE_METHOD(FrictionSystemFixture, "FrictionSystem allows stronger friction when frictionReduction is negative",
    "[unit][friction_system]"
) {
    auto& comp = this->world.components();
    const auto entity = this->world.entities().create();

    comp.add<VelocityComponent>(entity, VelocityComponent{ 10.f, 0.f });
    comp.add<GroundedComponent>(entity, GroundedComponent{ true, -0.5f });

    this->context.deltaTime = 1.f;
    this->system.update(this->context);

    const auto& v = comp.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == Catch::Approx(8.5f));
}

TEST_CASE_METHOD(FrictionSystemFixture, "FrictionSystem ignores entities frozen by hitstop",
    "[integration][friction_system]"
) {
    auto& comp = this->world.components();
    const auto entity = this->world.entities().create();

    comp.add<VelocityComponent>(entity, VelocityComponent{ 10.f, 0.f });
    comp.add<GroundedComponent>(entity, GroundedComponent{ true, 0.f });
    comp.add<HitstopComponent>(entity, HitstopComponent{ .remaining = 1.f, .frozen = true });

    this->context.deltaTime = 1.f;
    this->system.update(this->context);

    const auto& v = comp.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == Catch::Approx(10.f));
}
