#include "UIFlexLayoutSystem/UIFlexLayoutSystem.h"

#include "FlexLayoutContext/FlexLayoutContext.h"

#include "domain/components/FlexContainer.h"
#include "domain/components/LayoutDirtyComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/View/View.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

void UIFlexLayoutSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<FlexContainer>(comp);

    for (auto [entity, flex] : view)
    {
        if (!comp.has<LayoutDirtyComponent>(entity)) continue;
        if (!comp.get<LayoutDirtyComponent>(entity).dirty) continue;
        if (this->hasFlexParent(ctx.world, entity)) continue;

        this->layoutRecursive(ctx.world, entity);
    }
}

void UIFlexLayoutSystem::addHandler(std::unique_ptr<IFlexLayoutHandler> handler)
{ this->handlers.push_back(std::move(handler)); }

bool UIFlexLayoutSystem::hasFlexParent(World& world, Entity entity) const
{
    auto& comp = world.components();
    if (!comp.has<ParentComponent>(entity)) return false;
    Entity parent = comp.get<ParentComponent>(entity).parent;
    return comp.has<FlexContainer>(parent);
}

std::vector<Entity> UIFlexLayoutSystem::collectChildren(World& world, Entity parent) const
{
    std::vector<Entity> result;
    auto view = View<ParentComponent>(world.components());
    for (auto [entity, parentComp] : view) if (parentComp.parent == parent) result.push_back(entity);
    return result;
}

void UIFlexLayoutSystem::layoutRecursive(World& world, Entity container)
{
    this->layoutContainer(world, container);
    auto& comp = world.components();
    comp.get<LayoutDirtyComponent>(container).dirty = false;

    for (Entity child : this->collectChildren(world, container)) if (comp.has<FlexContainer>(child))
    { this->layoutRecursive(world, child); }
}

void UIFlexLayoutSystem::layoutContainer(World& world, Entity container)
{
    auto& comp = world.components();
    auto& flex = comp.get<FlexContainer>(container);
    auto children = this->collectChildren(world, container);
    if (children.empty()) return;

    FlexLayoutContext ctx{
        .world      = world,
        .container  = container,
        .flex       = flex,
        .children   = children,
    };

    for (auto& handler : this->handlers) handler->layout(ctx);
}
