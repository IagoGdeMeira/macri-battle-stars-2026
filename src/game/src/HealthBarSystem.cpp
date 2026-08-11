#include "HealthBarSystem/HealthBarSystem.h"

#include "domain/components/ChildrenComponent.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/UITransform.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

HealthBarSystem::HealthBarSystem(EventBus& bus) : bus(bus)
{
    this->bus.subscribe<DamageEvent>([this](const DamageEvent& e)
    { this->damageEvents.push_back(e); });
}

void HealthBarSystem::update(UpdateContext& ctx)
{
    for (const auto& event : this->damageEvents) this->processDamageEvent(ctx.world, event);

    this->damageEvents.clear();
}

void HealthBarSystem::processDamageEvent(World& world, const DamageEvent& event)
{
    Entity container = this->findHealthBarContainer(world, event.targetPlayerId);
    if (container == Entity::Invalid) return;

    auto& comp = world.components();
    auto& tag = comp.get<HealthBarTag>(container);
    tag.currentHealth = event.remainingHealth;
    if (tag.currentHealth < 0) tag.currentHealth = 0;

    this->updateHealthBarSegments(world, container, tag.currentHealth);
}

Entity HealthBarSystem::findHealthBarContainer(World& world, uint32_t playerId) const
{
    auto& comp = world.components();
    auto view = View<HealthBarTag>(comp);
    for (auto [entity, tag] : view)
        if (tag.playerId == playerId) return entity;
    return Entity::Invalid;
}

void HealthBarSystem::updateHealthBarSegments(World& world, Entity container, int currentHealth)
{
    auto& comp = world.components();
    if (!comp.has<ChildrenComponent>(container)) return;

    const auto& children = comp.get<ChildrenComponent>(container).children;
    float remainingHP = static_cast<float>(currentHealth);

    for (Entity child : children.children)
    {
        if (!comp.has<HealthBarSegmentComponent>(child)) continue;

        auto& segmentComp = comp.get<HealthBarSegmentComponent>(child);
        float maxHP = segmentComp.maxHP;
        float segmentHP = std::min(remainingHP, maxHP);
        remainingHP -= segmentHP;

        this->updateSegmentWidth(world, child, segmentHP);
    }
}

void HealthBarSystem::updateSegmentWidth(World& world, Entity segment, float currentHP)
{
    auto& comp = world.components();
    auto& segmentComp = comp.get<HealthBarSegmentComponent>(segment);
    auto& transform = comp.get<UITransform>(segment);
    transform.rect.size.width = (currentHP / segmentComp.maxHP) * segmentComp.maxWidth;
}