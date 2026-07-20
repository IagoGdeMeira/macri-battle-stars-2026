#include "game/include/HitstopSystem/HitstopSystem.h"

#include "domain/components/HitstopComponent.h"
#include "domain/events/DamageEvent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class HitstopSystemFixture
{
public:
    HitstopSystemFixture() : system(bus), context { this->world, this->bus, this->commandBuffer, 0.f }
    { this->world.components().registerComponent<HitstopComponent>(); }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    HitstopSystem system;
    UpdateContext context;
};

TEST_CASE_METHOD(HitstopSystemFixture, "HitstopSystem freezes attacker and target on damage",
    "[unit][hitstop_system]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    comp.add<HitstopComponent>(attacker, HitstopComponent{});
    comp.add<HitstopComponent>(target, HitstopComponent{});

    DamageEvent damage { .attacker = attacker, .target = target, .damage = 10 };
    this->bus.emit<DamageEvent>(damage);

    this->context.deltaTime = 0.016f;
    this->system.update(this->context);

    const auto& attackerHitstop = comp.get<HitstopComponent>(attacker);
    const auto& targetHitstop = comp.get<HitstopComponent>(target);

    REQUIRE(attackerHitstop.frozen);
    REQUIRE(targetHitstop.frozen);
    REQUIRE(attackerHitstop.remaining == Catch::Approx(0.084f));
    REQUIRE(targetHitstop.remaining == Catch::Approx(0.084f));
}

TEST_CASE_METHOD(HitstopSystemFixture, "HitstopSystem ignores entities without HitstopComponent",
    "[unit][hitstop_system]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();
    auto spectator = entities.create();

    comp.add<HitstopComponent>(target, HitstopComponent{});
    comp.add<HitstopComponent>(spectator, HitstopComponent{ .remaining = 0.25f, .frozen = false });

    DamageEvent damage { .attacker = attacker, .target = target, .damage = 10 };
    this->bus.emit<DamageEvent>(damage);

    this->context.deltaTime = 0.016f;
    this->system.update(this->context);

    const auto& targetHitstop = comp.get<HitstopComponent>(target);
    const auto& spectatorHitstop = comp.get<HitstopComponent>(spectator);

    REQUIRE(targetHitstop.frozen);
    REQUIRE(targetHitstop.remaining == Catch::Approx(0.084f));
    REQUIRE(spectatorHitstop.frozen == false);
    REQUIRE(spectatorHitstop.remaining == Catch::Approx(0.25f));
}

TEST_CASE_METHOD(HitstopSystemFixture, "HitstopSystem releases freeze when remaining time expires",
    "[unit][hitstop_system]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    comp.add<HitstopComponent>(attacker, HitstopComponent{});
    comp.add<HitstopComponent>(target, HitstopComponent{});

    DamageEvent damage { .attacker = attacker, .target = target, .damage = 10 };
    this->bus.emit<DamageEvent>(damage);

    this->context.deltaTime = 0.2f;
    this->system.update(this->context);

    const auto& attackerHitstop = comp.get<HitstopComponent>(attacker);
    const auto& targetHitstop = comp.get<HitstopComponent>(target);

    REQUIRE(attackerHitstop.frozen == false);
    REQUIRE(targetHitstop.frozen == false);
    REQUIRE(attackerHitstop.remaining == Catch::Approx(0.f));
    REQUIRE(targetHitstop.remaining == Catch::Approx(0.f));
}

TEST_CASE_METHOD(HitstopSystemFixture, "HitstopSystem keeps frozen entities frozen across partial updates",
    "[unit][hitstop_system]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    auto attacker = entities.create();
    auto target = entities.create();

    comp.add<HitstopComponent>(attacker, HitstopComponent{});
    comp.add<HitstopComponent>(target, HitstopComponent{});

    DamageEvent damage { .attacker = attacker, .target = target, .damage = 10 };
    this->bus.emit<DamageEvent>(damage);

    this->context.deltaTime = 0.03f;
    this->system.update(this->context);
    this->system.update(this->context);

    const auto& attackerHitstop = comp.get<HitstopComponent>(attacker);
    const auto& targetHitstop = comp.get<HitstopComponent>(target);

    REQUIRE(attackerHitstop.frozen);
    REQUIRE(targetHitstop.frozen);
    REQUIRE(attackerHitstop.remaining == Catch::Approx(0.04f));
    REQUIRE(targetHitstop.remaining == Catch::Approx(0.04f));
}
