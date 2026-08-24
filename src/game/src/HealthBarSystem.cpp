#include "HealthBarSystem/HealthBarSystem.h"

#include "domain/components/ChildrenComponent.h"
#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <unordered_map>
#include <vector>

HealthBarSystem::HealthBarSystem(EventBus& bus) : bus(bus)
{
    this->bus.subscribe<DamageEvent>([this](const DamageEvent& e)
    { this->damageEvents.push_back(e); });
}

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
    float remainingHP = static_cast<float>(currentHealth);

    std::unordered_map<int, std::vector<Entity>> segmentsByIndex;

    for (Entity child : children)
    {
        if (!comp.has<HealthBarSegmentComponent>(child)) continue;
        int idx = comp.get<HealthBarSegmentComponent>(child).segmentIndex;
        segmentsByIndex[idx].push_back(child);
    }

    for (auto& [index, entities] : segmentsByIndex)
    {
        if (entities.empty()) continue;

        auto& firstSeg = comp.get<HealthBarSegmentComponent>(entities.front());
        float maxHP = firstSeg.maxHP;
        float segmentHP = std::min(remainingHP, maxHP);
        remainingHP -= segmentHP;

        float newWidth = (segmentHP / maxHP) * firstSeg.maxWidth;

        for (Entity e : entities) this->updateSegmentWidth(world, e, newWidth);
    }
}

void HealthBarSystem::updateSegmentWidth(World& world, Entity segment, float newWidth)
{
    auto& comp = world.components();

    if (comp.has<UILayoutMetricsComponent>(segment)) comp.get<UILayoutMetricsComponent>(segment).size.width = newWidth;
    if (comp.has<RectangleShapeComponent>(segment)) comp.get<RectangleShapeComponent>(segment).rect.size.width = newWidth;
}
