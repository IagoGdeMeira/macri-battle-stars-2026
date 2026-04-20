#include "../../src/game/include/GravitySystem/GravitySystem.h"

#include "../../src/domain/components/GravityComponent.h"
#include "../../src/domain/components/VelocityComponent.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class GravitySystemFixture
{
public:
    GravitySystemFixture() : system(10.0f), context { world, bus, commandBuffer, 0.0f }
    {
        auto& components = this->world.components();
        components.registerComponent<VelocityComponent>();
        components.registerComponent<GravityComponent>();
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
    auto& components = this->world.components();
    components.add<VelocityComponent>(entity, VelocityComponent{ 1.0f, 2.0f });
    components.add<GravityComponent>(entity, GravityComponent{ 1.5f });

    this->context.deltaTime = 0.2f;
    this->system.update(this->context);

    const auto& velocity = components.get<VelocityComponent>(entity);
    REQUIRE(velocity.vx == 1.0f);
    REQUIRE(velocity.vy == Catch::Approx(5.0f));
}

TEST_CASE_METHOD(GravitySystemFixture, "GravitySystem updates only entities that have VelocityComponent and GravityComponent",
    "[unit][gravity_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();
    
    const auto withBoth = entities.create();
    components.add<VelocityComponent>(withBoth, VelocityComponent{ 0.0f, 3.0f });
    components.add<GravityComponent>(withBoth, GravityComponent{ 2.0f });

    const auto withVelocityOnly = entities.create();
    components.add<VelocityComponent>(withVelocityOnly, VelocityComponent{ 4.0f, 6.0f });

    const auto withGravityOnly = entities.create();
    components.add<GravityComponent>(withGravityOnly, GravityComponent{ 3.0f });

    this->context.deltaTime = 0.5f;
    this->system.update(this->context);

    const auto& velocityWithBoth = components.get<VelocityComponent>(withBoth);
    const auto& velocityOnly = components.get<VelocityComponent>(withVelocityOnly);

    REQUIRE(velocityWithBoth.vy == Catch::Approx(13.0f));
    REQUIRE(velocityOnly.vx == 4.0f);
    REQUIRE(velocityOnly.vy == 6.0f);
    REQUIRE(components.has<GravityComponent>(withGravityOnly));
}

TEST_CASE_METHOD(GravitySystemFixture, "GravitySystem supports negative gravity scale",
    "[unit][gravity_system]"
) {
    const auto entity = this->world.entities().create();
    auto& components = this->world.components();

    components.add<VelocityComponent>(entity, VelocityComponent{ 0.0f, 10.0f });
    components.add<GravityComponent>(entity, GravityComponent{ -0.5f });

    this->context.deltaTime = 0.4f;
    this->system.update(this->context);

    const auto& velocity = components.get<VelocityComponent>(entity);
    REQUIRE(velocity.vy == Catch::Approx(8.0f));
}
