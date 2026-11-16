#include "game/include/HealthBarSystem/HealthBarSystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/HealthBarEndCapComponent.h"
#include "domain/components/HealthBarLayoutComponent.h"
#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/PolygonShapeComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include "game/events/DamageEvent.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

class HealthBarSystemFixture
{
public:
    HealthBarSystemFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<ActiveComponent>();
        comp.registerComponent<ChildrenComponent>();
        comp.registerComponent<HealthBarEndCapComponent>();
        comp.registerComponent<HealthBarLayoutComponent>();
        comp.registerComponent<HealthBarSegmentComponent>();
        comp.registerComponent<HealthBarTag>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<PolygonShapeComponent>();
        comp.registerComponent<RectangleShapeComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UILayoutMetricsComponent>();
    }

    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    HealthBarSystem system{this->bus};
    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};

    Entity createHealthBar(
        uint32_t playerId, int maxHealth, int currentHealth,
        float totalWidth = 300.f, float barHeight = 30.f
    ) {
        auto& comp = this->world.components();
        Entity container = this->world.entities().create();

        comp.add<TransformComponent>(container, TransformComponent{Position{0.f, 0.f}});
        comp.add<UILayoutMetricsComponent>(container, UILayoutMetricsComponent{Dimension2D{totalWidth, barHeight}});
        comp.add<ActiveComponent>(container, ActiveComponent{true});
        comp.add<ChildrenComponent>(container, ChildrenComponent{});
        comp.add<HealthBarTag>(container, HealthBarTag{playerId, maxHealth, currentHealth});

        const float maxSegmentHP = 100.f;
        int numSegments = (maxHealth + static_cast<int>(maxSegmentHP) - 1) / static_cast<int>(maxSegmentHP);
        float remainingHP = static_cast<float>(currentHealth);

        auto& children = comp.get<ChildrenComponent>(container);
        for (int i = 0; i < numSegments; ++i)
        {
            float segmentHP = std::min(remainingHP, maxSegmentHP);
            remainingHP -= segmentHP;

            float segmentWidth = (segmentHP / maxSegmentHP) * totalWidth;

            Entity segment = this->world.entities().create();
            comp.add<TransformComponent>(segment, TransformComponent{Position{0.f, 0.f}});
            comp.add<UILayoutMetricsComponent>(segment, UILayoutMetricsComponent{Dimension2D{segmentWidth, barHeight}});
            comp.add<ActiveComponent>(segment, ActiveComponent{true});
            comp.add<ParentComponent>(segment, ParentComponent{container});
            comp.add<HealthBarSegmentComponent>(segment, HealthBarSegmentComponent{
                maxSegmentHP, totalWidth, Color::WHITE(), Color::WHITE(), i});
            children.children.push_back(segment);
        }
        return container;
    }
};

TEST_CASE_METHOD(HealthBarSystemFixture, "HealthBarSystem reduces segment width when damage is received", "[unit][health_bar_system]")
{
    Entity bar = this->createHealthBar(0, 300, 300);

    auto& entities = this->world.entities();
    Entity attacker = entities.create(), target = entities.create();
    this->bus.emit<DamageEvent>(DamageEvent{attacker, target, 0, 50, 250});

    this->system.update(this->ctx);

    auto& comp = this->world.components();
    const auto& children = comp.get<ChildrenComponent>(bar).children;

    {
        auto& uiRect = comp.get<UILayoutMetricsComponent>(children[0]);
        auto& seg = comp.get<HealthBarSegmentComponent>(children[0]);
        REQUIRE(uiRect.size.width == seg.maxWidth);
    }

    {
        auto& uiRect = comp.get<UILayoutMetricsComponent>(children[1]);
        auto& seg = comp.get<HealthBarSegmentComponent>(children[1]);
        REQUIRE(uiRect.size.width == seg.maxWidth);
    }

    {
        auto& uiRect = comp.get<UILayoutMetricsComponent>(children[2]);
        auto& seg = comp.get<HealthBarSegmentComponent>(children[2]);
        REQUIRE(uiRect.size.width == (50.f / seg.maxHP) * seg.maxWidth);
    }
}

TEST_CASE_METHOD(HealthBarSystemFixture, "HealthBarSystem handles zero health correctly",
    "[unit][health_bar_system]")
{
    Entity bar = this->createHealthBar(0, 200, 200);

    auto& entities = this->world.entities();
    Entity attacker = entities.create(), target = entities.create();
    this->bus.emit<DamageEvent>(DamageEvent{attacker, target, 0, 200, 0});

    this->system.update(this->ctx);

    auto& comp = this->world.components();
    const auto& children = comp.get<ChildrenComponent>(bar).children;

    for (Entity child : children)
    {
        auto& uiRect = comp.get<UILayoutMetricsComponent>(child);
        REQUIRE(uiRect.size.width == 0.f);
    }
}

TEST_CASE_METHOD(HealthBarSystemFixture, "HealthBarSystem ignores events for different player", "[unit][health_bar_system]")
{
    this->createHealthBar(0, 100, 100);

    auto& entities = this->world.entities();
    Entity attacker = entities.create(), target = entities.create();
    this->bus.emit<DamageEvent>(DamageEvent{attacker, target, 1, 30, 70});

    this->system.update(this->ctx);

    auto& comp = this->world.components();
    auto view = View<HealthBarTag>(comp);
    for (auto [entity, tag] : view) REQUIRE(tag.currentHealth == 100);
}

TEST_CASE_METHOD(HealthBarSystemFixture, "HealthBarSystem updates multiple bars independently",
    "[unit][health_bar_system]")
{
    this->createHealthBar(0, 200, 200);
    this->createHealthBar(1, 100, 100);

    auto& entities = this->world.entities();
    Entity attacker1 = entities.create(), target1 = entities.create();
    Entity attacker2 = entities.create(), target2 = entities.create();

    this->bus.emit<DamageEvent>(DamageEvent{attacker1, target1, 0, 50, 150});
    this->bus.emit<DamageEvent>(DamageEvent{attacker2, target2, 1, 20, 80});

    this->system.update(this->ctx);

    auto& comp = this->world.components();
    auto view = View<HealthBarTag>(comp);
    for (auto [entity, tag] : view)
    {
        if (tag.playerId == 0) REQUIRE(tag.currentHealth == 150);
        else if (tag.playerId == 1) REQUIRE(tag.currentHealth == 80);
    }
}

TEST_CASE_METHOD(HealthBarSystemFixture, "HealthBarSystem clamps negative health to zero", "[unit][health_bar_system]")
{
    this->createHealthBar(0, 50, 50);
    auto& entities = this->world.entities();
    Entity attacker = entities.create(), target = entities.create();
    this->bus.emit<DamageEvent>(DamageEvent{attacker, target, 0, 100, -10});

    this->system.update(this->ctx);

    auto& comp = this->world.components();
    auto view = View<HealthBarTag>(comp);
    for (auto [entity, tag] : view) REQUIRE(tag.currentHealth == 0);
}

TEST_CASE_METHOD(HealthBarSystemFixture, "HealthBarSystem positions end cap at filled edge",
    "[unit][health_bar_system]")
{
    auto& comp = this->world.components();

    Entity bar = this->createHealthBar(0, 200, 100);

    comp.add<HealthBarLayoutComponent>(bar, HealthBarLayoutComponent{
        .flip = false,
        .barOffsetX = 0.f,
        .barHeight = 30.f,
        .capWidth = 10.f
    });

    Entity cap = this->world.entities().create();
    comp.add<TransformComponent>(cap, TransformComponent{Position{0.f, 0.f}});
    comp.add<ParentComponent>(cap, ParentComponent{bar});
    comp.add<PolygonShapeComponent>(cap, PolygonShapeComponent{});
    comp.add<HealthBarEndCapComponent>(cap, HealthBarEndCapComponent{});
    comp.get<ChildrenComponent>(bar).children.push_back(cap);

    this->system.update(this->ctx);

    const auto& capT = comp.get<TransformComponent>(cap);
    REQUIRE(capT.position.x == Catch::Approx(140.f));
}

TEST_CASE_METHOD(HealthBarSystemFixture, "HealthBarSystem mirrors end cap when flipped",
    "[unit][health_bar_system]")
{
    auto& comp = this->world.components();

    Entity bar = this->createHealthBar(0, 200, 100);

    comp.add<HealthBarLayoutComponent>(bar, HealthBarLayoutComponent{
        .flip = true,
        .barOffsetX = 0.f,
        .barHeight = 30.f,
        .capWidth = 10.f
    });

    Entity cap = this->world.entities().create();
    comp.add<TransformComponent>(cap, TransformComponent{Position{0.f, 0.f}});
    comp.add<ParentComponent>(cap, ParentComponent{bar});
    comp.add<PolygonShapeComponent>(cap, PolygonShapeComponent{});
    comp.add<HealthBarEndCapComponent>(cap, HealthBarEndCapComponent{});
    comp.get<ChildrenComponent>(bar).children.push_back(cap);

    this->system.update(this->ctx);

    const auto& capT = comp.get<TransformComponent>(cap);
    REQUIRE(capT.position.x == Catch::Approx(150.f));
}
