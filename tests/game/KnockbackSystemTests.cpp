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
    KnockbackSystemFixture() : system(this->bus), context { this->world, this->bus, this->commandBuffer, 0.016f }
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

    components.add<TransformComponent>(attacker, TransformComponent{0.f, 0.f});
    components.add<TransformComponent>(target, TransformComponent{10.f, 0.f});
    components.add<VelocityComponent>(target, VelocityComponent{0.f, 0.f});

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };

    this->bus.emit<DamageEvent>(dmg);
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(target);
    REQUIRE(v.velocity.x == Catch::Approx(300.f));
    REQUIRE(v.velocity.y == Catch::Approx(0.f));
}

TEST_CASE_METHOD(KnockbackSystemFixture, "KnockbackSystem normalizes direction vector",
    "[unit][knockback_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    components.add<TransformComponent>(attacker, TransformComponent{0.f, 0.f});
    components.add<TransformComponent>(target, TransformComponent{3.f, 4.f});
    components.add<VelocityComponent>(target, VelocityComponent{0.f, 0.f});

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };

    this->bus.emit<DamageEvent>(dmg);
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(target);
    REQUIRE(v.velocity.x == Catch::Approx(180.f));
    REQUIRE(v.velocity.y == Catch::Approx(240.f));
}

TEST_CASE_METHOD(KnockbackSystemFixture, "KnockbackSystem applies knockback multiplier from component",
    "[unit][knockback_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    components.add<TransformComponent>(attacker, TransformComponent{0.f, 0.f});
    components.add<TransformComponent>(target, TransformComponent{5.f, 0.f});
    components.add<VelocityComponent>(target, VelocityComponent{0.f, 0.f});
    components.add<KnockbackComponent>(target, KnockbackComponent{2.f, 0.5f});

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };

    this->bus.emit<DamageEvent>(dmg);
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(target);
    REQUIRE(v.velocity.x == Catch::Approx(300.f));
    REQUIRE(v.velocity.y == Catch::Approx(0.f));
}

TEST_CASE_METHOD(KnockbackSystemFixture, "KnockbackSystem handles zero distance (attacker same position as target)",
    "[unit][knockback_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    components.add<TransformComponent>(attacker, TransformComponent{5.f, 5.f});
    components.add<TransformComponent>(target, TransformComponent{5.f, 5.f});
    components.add<VelocityComponent>(target, VelocityComponent{10.f, 10.f});

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };

    this->bus.emit<DamageEvent>(dmg);
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(target);
    REQUIRE(v.velocity.x == 10.f);
    REQUIRE(v.velocity.y == 10.f);
}

TEST_CASE_METHOD(KnockbackSystemFixture,
    "KnockbackSystem ignores damage to entities without required components",
    "[unit][knockback_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    components.add<TransformComponent>(attacker, TransformComponent{ 0.f, 0.f });
    components.add<TransformComponent>(target, TransformComponent{ 10.f, 0.f });

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

    components.add<TransformComponent>(attacker, TransformComponent{0.f, 0.f});
    components.add<TransformComponent>(target, TransformComponent{10.f, 0.f});
    components.add<VelocityComponent>(target, VelocityComponent{100.f, 50.f});

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };

    this->bus.emit<DamageEvent>(dmg);
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(target);
    REQUIRE(v.velocity.x == Catch::Approx(400.f));
    REQUIRE(v.velocity.y == Catch::Approx(50.f));
}

TEST_CASE_METHOD(KnockbackSystemFixture, "KnockbackSystem handles diagonal knockback",
    "[unit][knockback_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    components.add<TransformComponent>(attacker, TransformComponent{0.f, 0.f});
    components.add<TransformComponent>(target, TransformComponent{1.f, 1.f});
    components.add<VelocityComponent>(target, VelocityComponent{0.f, 0.f});

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };

    this->bus.emit<DamageEvent>(dmg);
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(target);

    float normalized = 1.f / std::sqrt(2.f);
    REQUIRE(v.velocity.x == Catch::Approx(300.f * normalized));
    REQUIRE(v.velocity.y == Catch::Approx(300.f * normalized));
}

TEST_CASE_METHOD(KnockbackSystemFixture, "KnockbackSystem default multiplier is 1.0 when no KnockbackComponent",
    "[unit][knockback_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    components.add<TransformComponent>(attacker, TransformComponent{0.f, 0.f});
    components.add<TransformComponent>(target, TransformComponent{10.f, 0.f});
    components.add<VelocityComponent>(target, VelocityComponent{0.f, 0.f});

    DamageEvent dmg { .attacker = attacker, .target = target, .damage = 10 };

    this->bus.emit<DamageEvent>(dmg);
    this->system.update(this->context);

    const auto& v = components.get<VelocityComponent>(target);
    REQUIRE(v.velocity.x == Catch::Approx(300.f));
    REQUIRE(v.velocity.y == Catch::Approx(0.f));
}
