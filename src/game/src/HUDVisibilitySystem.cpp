#include "HUDVisibilitySystem/HUDVisibilitySystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/HUDEntityTag.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

void HUDVisibilitySystem::update(UpdateContext& ctx)
{
    if (this->targetVisibility == this->currentVisibility) return;

    auto& comp = ctx.world.components();
    auto view = View<ActiveComponent, HUDEntityTag>(comp);

    for (auto [entity, a_, t_] : view)
    {
        (void)t_; (void)a_;
        this->setActiveRecursive(ctx.world, entity, this->targetVisibility);
    }

    this->currentVisibility = this->targetVisibility;
}

void HUDVisibilitySystem::setVisible(bool visible) { this->targetVisibility = visible; }

void HUDVisibilitySystem::setActiveRecursive(World& world, Entity entity, bool active)
{
    auto& comp = world.components();
    if (comp.has<ActiveComponent>(entity)) comp.get<ActiveComponent>(entity).active = active;
    
    if (comp.has<ChildrenComponent>(entity)) for (Entity child : comp.get<ChildrenComponent>(entity).children)
    { this->setActiveRecursive(world, child, active); }
}
