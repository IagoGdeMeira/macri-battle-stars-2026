#include "HealthBarSystem/HealthBarSystem.h"

#include "domain/components/ChildrenComponent.h"
#include "domain/components/HealthBarEndCapComponent.h"
#include "domain/components/HealthBarLayoutComponent.h"
#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/PolygonShapeComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <map>
#include <vector>

HealthBarSystem::HealthBarSystem(EventBus& bus) : bus(bus)
{ this->bus.subscribe<DamageEvent>([this](const DamageEvent& e) { this->damageEvents.push_back(e); }); }

void HealthBarSystem::update(UpdateContext& ctx)
{
    for (const auto& event : this->damageEvents) this->processDamageEvent(ctx.world, event);
    this->damageEvents.clear();

    auto& comp = ctx.world.components();
    auto view = View<HealthBarTag>(comp);
    for (auto [entity, tag] : view) this->updateHealthBarSegments(ctx.world, entity, tag.currentHealth);
}

void HealthBarSystem::processDamageEvent(World& world, const DamageEvent& event)
{
    auto& comp = world.components();
    auto view = View<HealthBarTag>(comp);

    for (auto [entity, tag] : view)
    {
        if (tag.playerId != event.targetPlayerId) continue;

        tag.currentHealth = event.remainingHealth;
        if (tag.currentHealth < 0) tag.currentHealth = 0;

        this->updateHealthBarSegments(world, entity, tag.currentHealth);
        break;
    }
}

void HealthBarSystem::updateHealthBarSegments(World& world, Entity container, int currentHealth)
{
    auto& comp = world.components();
    if (!comp.has<ChildrenComponent>(container)) return;

    const auto& children = comp.get<ChildrenComponent>(container).children;

    std::map<int, std::vector<Entity>> segmentsByIndex;
    Entity endCap = Entity{0};
    bool hasCap = false;
    float barWidth = 0.f;
    Color lastActiveColor = Color::WHITE();

    for (Entity child : children)
    {
        if (comp.has<HealthBarSegmentComponent>(child))
        {
            const auto& seg = comp.get<HealthBarSegmentComponent>(child);
            if (barWidth == 0.f) barWidth = seg.maxWidth;
            segmentsByIndex[seg.segmentIndex].push_back(child);
        }
        if (comp.has<HealthBarEndCapComponent>(child))
        {
            endCap = child;
            hasCap = true;
        }
    }

    float remainingHP = static_cast<float>(currentHealth);

    for (auto& [index, entities] : segmentsByIndex)
    {
        if (entities.empty()) continue;

        auto& firstSeg = comp.get<HealthBarSegmentComponent>(entities.front());
        float maxHP = firstSeg.maxHP;
        float segmentHP = std::min(remainingHP, maxHP);
        remainingHP -= segmentHP;

        float newWidth = (segmentHP / maxHP) * firstSeg.maxWidth;

        for (Entity e : entities) this->updateSegmentWidth(world, e, newWidth);

        if (segmentHP > 0.f) lastActiveColor = firstSeg.fill;
    }

    EndCapData data{currentHealth, barWidth, lastActiveColor};
    if (hasCap) this->updateEndCap(world, container, endCap, data);
}

void HealthBarSystem::updateSegmentWidth(World& world, Entity segment, float newWidth)
{
    auto& comp = world.components();

    if (comp.has<UILayoutMetricsComponent>(segment)) comp.get<UILayoutMetricsComponent>(segment).size.width = newWidth;
    if (comp.has<RectangleShapeComponent>(segment)) comp.get<RectangleShapeComponent>(segment).rect.size.width = newWidth;
}

void HealthBarSystem::updateEndCap(World& world, Entity container, Entity endCap, EndCapData data)
{
    auto& comp = world.components();
    if (!comp.has<HealthBarLayoutComponent>(container)) return;
    if (!comp.has<HealthBarTag>(container)) return;

    const auto& layout = comp.get<HealthBarLayoutComponent>(container);
    const auto& tag = comp.get<HealthBarTag>(container);

    float fraction = (tag.maxHealth > 0)
        ? std::max(0.f, std::min(1.f, static_cast<float>(data.currentHealth) / static_cast<float>(tag.maxHealth)))
        : 0.f;
    float filledWidth = data.barWidth * fraction;

    auto& capTransform = comp.get<TransformComponent>(endCap);
    if (layout.flip) capTransform.position.x = layout.barOffsetX + data.barWidth - filledWidth;
    else capTransform.position.x = layout.barOffsetX + filledWidth - layout.capWidth;

    if (comp.has<PolygonShapeComponent>(endCap)) comp.get<PolygonShapeComponent>(endCap).color = data.color;
}
