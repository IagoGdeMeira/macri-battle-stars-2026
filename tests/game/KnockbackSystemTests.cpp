#include "../../src/game/include/KnockbackSystem/KnockbackSystem.h"

#include "../../src/domain/components/TransformComponent.h"
#include "../../src/domain/components/VelocityComponent.h"
#include "../../src/domain/components/KnockbackComponent.h"
#include "../../src/domain/events/DamageEvent.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>

class KnockbackSystemFixture
{
public:
    KnockbackSystemFixture() : system(bus), context { world, bus, commandBuffer, 0.016f }
    {
        auto& components = this->world.components();
        components.registerComponent<TransformComponent>();
        components.registerComponent<VelocityComponent>();
        components.registerComponent<KnockbackComponent>();
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    KnockbackSystem system;
    UpdateContext context;
};

TEST_CASE_METHOD(KnockbackSystemFixture, "KnockbackSystem applies knockback in correct direction",
    "[unit][knockback_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    components.add<TransformComponent>(attacker, TransformComponent{ 0.0f, 0.0f });
    components.add<TransformComponent>(target, TransformComponent{ 10.0f, 0.0f });
    components.add<VelocityComponent>(target, VelocityComponent{ 0.0f, 0.0f });

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };

    this->bus.emit<DamageEvent>(dmg);
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(target);
    REQUIRE(v.velocity.x == Catch::Approx(300.0f));
    REQUIRE(v.velocity.y == Catch::Approx(0.0f));
}

TEST_CASE_METHOD(KnockbackSystemFixture, "KnockbackSystem normalizes direction vector",
    "[unit][knockback_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    components.add<TransformComponent>(attacker, TransformComponent{ 0.0f, 0.0f });
    components.add<TransformComponent>(target, TransformComponent{ 3.0f, 4.0f });
    components.add<VelocityComponent>(target, VelocityComponent{ 0.0f, 0.0f });

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };

    this->bus.emit<DamageEvent>(dmg);
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(target);
    REQUIRE(v.velocity.x == Catch::Approx(180.0f));
    REQUIRE(v.velocity.y == Catch::Approx(240.0f));
}

TEST_CASE_METHOD(KnockbackSystemFixture, "KnockbackSystem applies knockback multiplier from component",
    "[unit][knockback_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    components.add<TransformComponent>(attacker, TransformComponent{ 0.0f, 0.0f });
    components.add<TransformComponent>(target, TransformComponent{ 5.0f, 0.0f });
    components.add<VelocityComponent>(target, VelocityComponent{ 0.0f, 0.0f });
    components.add<KnockbackComponent>(target, KnockbackComponent{ 2.0f, 0.5f });

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };

    this->bus.emit<DamageEvent>(dmg);
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(target);
    REQUIRE(v.velocity.x == Catch::Approx(300.0f));
    REQUIRE(v.velocity.y == Catch::Approx(0.0f));
}

TEST_CASE_METHOD(KnockbackSystemFixture, "KnockbackSystem handles zero distance (attacker same position as target)",
    "[unit][knockback_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    components.add<TransformComponent>(attacker, TransformComponent{ 5.0f, 5.0f });
    components.add<TransformComponent>(target, TransformComponent{ 5.0f, 5.0f });
    components.add<VelocityComponent>(target, VelocityComponent{ 10.0f, 10.0f });

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };

    this->bus.emit<DamageEvent>(dmg);
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(target);
    REQUIRE(v.velocity.x == 10.0f);
    REQUIRE(v.velocity.y == 10.0f);
}

TEST_CASE_METHOD(KnockbackSystemFixture, "KnockbackSystem ignores damage to entities without required components",
    "[unit][knockback_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    components.add<TransformComponent>(attacker, TransformComponent{ 0.0f, 0.0f });
    components.add<TransformComponent>(target, TransformComponent{ 10.0f, 0.0f });

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };
    this->bus.emit<DamageEvent>(dmg);

    REQUIRE_NOTHROW(this->system.update(this->context));
}

TEST_CASE_METHOD(KnockbackSystemFixture, "KnockbackSystem accumulates velocity from multiple hits",
    "[unit][knockback_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    components.add<TransformComponent>(attacker, TransformComponent{ 0.0f, 0.0f });
    components.add<TransformComponent>(target, TransformComponent{ 10.0f, 0.0f });
    components.add<VelocityComponent>(target, VelocityComponent{ 100.0f, 50.0f });

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };

    this->bus.emit<DamageEvent>(dmg);
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(target);
    REQUIRE(v.velocity.x == Catch::Approx(400.0f));
    REQUIRE(v.velocity.y == Catch::Approx(50.0f));
}

TEST_CASE_METHOD(KnockbackSystemFixture, "KnockbackSystem handles diagonal knockback",
    "[unit][knockback_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    components.add<TransformComponent>(attacker, TransformComponent{ 0.0f, 0.0f });
    components.add<TransformComponent>(target, TransformComponent{ 1.0f, 1.0f });
    components.add<VelocityComponent>(target, VelocityComponent{ 0.0f, 0.0f });

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };

    this->bus.emit<DamageEvent>(dmg);
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(target);

    float normalized = 1.0f / std::sqrt(2.0f);
    REQUIRE(v.velocity.x == Catch::Approx(300.0f * normalized));
    REQUIRE(v.velocity.y == Catch::Approx(300.0f * normalized));
}

TEST_CASE_METHOD(KnockbackSystemFixture, "KnockbackSystem default multiplier is 1.0 when no KnockbackComponent",
    "[unit][knockback_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    components.add<TransformComponent>(attacker, TransformComponent{ 0.0f, 0.0f });
    components.add<TransformComponent>(target, TransformComponent{ 10.0f, 0.0f });
    components.add<VelocityComponent>(target, VelocityComponent{ 0.0f, 0.0f });

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };

    this->bus.emit<DamageEvent>(dmg);
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(target);
    REQUIRE(v.velocity.x == Catch::Approx(300.0f));
    REQUIRE(v.velocity.y == Catch::Approx(0.0f));
}
