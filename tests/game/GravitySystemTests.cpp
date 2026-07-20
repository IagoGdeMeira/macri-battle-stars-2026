#include "game/include/GravitySystem/GravitySystem.h"

#include "domain/components/GravityComponent.h"
#include "domain/components/HitstopComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class GravitySystemFixture
{
public:
    GravitySystemFixture() : system(10.f), context { this->world, this->bus, this->commandBuffer, 0.f }
    {
        auto& comp = this->world.components();
        comp.registerComponent<VelocityComponent>();
        comp.registerComponent<GravityComponent>();
        comp.registerComponent<HitstopComponent>();
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    GravitySystem system;
    UpdateContext context;
};

TEST_CASE_METHOD(GravitySystemFixture, "GravitySystem updates vertical velocity using delta time and gravity scale",
    "[unit][gravity_system]"
) {
    const auto entity = this->world.entities().create();
    auto& comp = this->world.components();
    comp.add<VelocityComponent>(entity, VelocityComponent{ 1.f, 2.f });
    comp.add<GravityComponent>(entity, GravityComponent{ 1.5f });

    this->context.deltaTime = 0.2f;
    this->system.update(this->context);

    const auto& v = comp.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.x == 1.f);
    REQUIRE(v.velocity.y == Catch::Approx(5.f));
}

TEST_CASE_METHOD(GravitySystemFixture, "GravitySystem updates only entities that have VelocityComponent and GravityComponent",
    "[unit][gravity_system]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();
    
    const auto withBoth = entities.create();
    comp.add<VelocityComponent>(withBoth, VelocityComponent{ 0.f, 3.f });
    comp.add<GravityComponent>(withBoth, GravityComponent{ 2.f });

    const auto withVelocityOnly = entities.create();
    comp.add<VelocityComponent>(withVelocityOnly, VelocityComponent{ 4.f, 6.f });

    const auto withGravityOnly = entities.create();
    comp.add<GravityComponent>(withGravityOnly, GravityComponent{ 3.f });

    this->context.deltaTime = 0.5f;
    this->system.update(this->context);

    const auto& velocityWithBoth = comp.get<VelocityComponent>(withBoth);
    const auto& velocityOnly = comp.get<VelocityComponent>(withVelocityOnly);

    REQUIRE(velocityWithBoth.velocity.y == Catch::Approx(13.f));
    REQUIRE(velocityOnly.velocity.x == 4.f);
    REQUIRE(velocityOnly.velocity.y == 6.f);
    REQUIRE(comp.has<GravityComponent>(withGravityOnly));
}

TEST_CASE_METHOD(GravitySystemFixture, "GravitySystem supports negative gravity scale",
    "[unit][gravity_system]"
) {
    const auto entity = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<VelocityComponent>(entity, VelocityComponent{ 0.f, 10.f });
    comp.add<GravityComponent>(entity, GravityComponent{ -0.5f });

    this->context.deltaTime = 0.4f;
    this->system.update(this->context);

    const auto& v = comp.get<VelocityComponent>(entity);
    REQUIRE(v.velocity.y == Catch::Approx(8.f));
}
