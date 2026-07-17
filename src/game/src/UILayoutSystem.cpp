#include "../include/UILayoutSystem/UILayoutSystem.h"

#include "../../domain/components/BoxModel.h"
#include "../../domain/components/FlexContainer.h"
#include "../../domain/components/FlexItem.h"
#include "../../domain/components/ParentComponent.h"
#include "../../domain/components/UITransform.h"
#include "../../domain/include/View/View.h"

#include "../../engine/value_objects/UpdateContext/UpdateContext.h"

#include <algorithm>

void UILayoutSystem::update(UpdateContext& ctx)
{
    auto containers = View<FlexContainer, UITransform>(ctx.world.components());
    for (auto [entity, flex, transform] : containers)
    {
        if (!flex.needsLayout) continue;
        
        this->layoutContainer({ctx, entity});
        flex.needsLayout = false;
    }
}

std::vector<Entity> UILayoutSystem::collectChildren(Item item)
{
    std::vector<Entity> children;

    auto view = View<ParentComponent>(item.ctx.world.components());

    for (auto [entity, parentComp] : view)
    { if (parentComp.parent == item.entity) children.push_back(entity); }

    return children;
}

void UILayoutSystem::applyPadding(Item item, Rectangle& innerRect) const
{
    AABB padding {0.f, 0.f, 0.f, 0.f};

    auto& components = item.ctx.world.components();
    if (components.has<BoxModel>(item.entity))
    {
        auto& box = components.get<BoxModel>(item.entity);
        padding.left = box.border.top + box.padding.top;
        padding.top = box.border.left + box.padding.left;
        padding.right = box.border.right + box.padding.right;
        padding.bottom = box.border.bottom + box.padding.bottom;
    }

    auto& transform = components.get<UITransform>(item.entity);
    innerRect.position.x = transform.rect.position.x + padding.left;
    innerRect.position.y = transform.rect.position.y + padding.top;
    innerRect.size.width = transform.rect.size.width - padding.left - padding.right;
    innerRect.size.height = transform.rect.size.height - padding.top - padding.bottom;
}

float UILayoutSystem::calculateFreeSpace(Item item, ItemGap& itemGap, float mainSize)
{
    float total = 0.f;

    auto children = this->collectChildren({item.ctx, item.entity});
    for (auto child : children)
    {
        auto& transform = item.ctx.world.components().get<UITransform>(child);
        total += itemGap.isColumn ? transform.rect.size.height : transform.rect.size.width;
    }

    if (children.size() > 1) total += itemGap.gap * (children.size() - 1);
    return mainSize - total;
}

void UILayoutSystem::distributeMainAxis(Item item, ItemGap& itemGap, DistributionParams& params)
{
    float between = 0, startOffset = 0;
    auto orderedChildren = this->collectChildren({item.ctx, item.entity});

    using Justify = FlexContainer::JustifyContent;
    switch (params.justify)
    {
        case Justify::FlexEnd: startOffset = params.freeSpace; break;
        case Justify::Center: startOffset = params.freeSpace * 0.5f; break;
        case Justify::SpaceBetween:
            if (orderedChildren.size() > 1) between = params.freeSpace / (orderedChildren.size() - 1);
            break;
        case Justify::SpaceAround: 
            if (orderedChildren.empty()) break;
            between = params.freeSpace / orderedChildren.size();
            startOffset = between * 0.5f;
            break;
        default: break;
    }

    float cursor = params.mainPos + startOffset;
    for (size_t i = 0; i < orderedChildren.size(); ++i)
    {
        this->positionItemMenu({item.ctx, orderedChildren[i]}, cursor, itemGap);
        if (i < orderedChildren.size() - 1) cursor += between;
    }
}

void UILayoutSystem::positionItemMenu(Item item, float& cursor, ItemGap& itemGap)
{
    auto& components = item.ctx.world.components();
    auto& itemTransform = components.get<UITransform>(item.entity);
    BoxModel* itemBox = components.has<BoxModel>(item.entity) ? &components.get<BoxModel>(item.entity) : nullptr;

    if (itemGap.isColumn)
    {
        itemTransform.rect.position.y = cursor + (itemBox ? itemBox->margin.top : 0.f);
        cursor += itemTransform.rect.size.height + (itemBox ? itemBox->margin.top + itemBox->margin.bottom : 0.f) + itemGap.gap;
    }
    else
    {
        itemTransform.rect.position.x = cursor + (itemBox ? itemBox->margin.left : 0.f);
        cursor += itemTransform.rect.size.width + (itemBox ? itemBox->margin.left + itemBox->margin.right : 0.f) + itemGap.gap;
    }
}

void UILayoutSystem::alignItemCross(Item item, AlignmentParams& params)
{
    auto& components = item.ctx.world.components();
    auto& itemTransform = components.get<UITransform>(item.entity);

    float itemCrossSize = params.isColumn ? itemTransform.rect.size.width : itemTransform.rect.size.height;
    
    BoxModel* itemBox = components.has<BoxModel>(item.entity) ? &components.get<BoxModel>(item.entity) : nullptr;
    float marginStart = itemBox ? (params.isColumn ? itemBox->margin.left : itemBox->margin.top) : 0.f;

    float pos = params.crossPos + marginStart;

    using Align = FlexContainer::AlignItems;
    if (params.align == Align::Center) pos += (params.crossSize - itemCrossSize) * 0.5f;
    else if (params.align == Align::FlexEnd) pos += params.crossSize - itemCrossSize;

    if (params.isColumn) itemTransform.rect.position.x = pos;
    else itemTransform.rect.position.y = pos;
}

void UILayoutSystem::layoutContainer(Item item)
{
    auto& components = item.ctx.world.components();
    auto& flex = components.get<FlexContainer>(item.entity);

    auto children = this->collectChildren(item);

    Rectangle innerRect;
    this->applyPadding(item, innerRect);

    using Dir = FlexContainer::FlexDirection;
    bool isColumn = flex.direction == Dir::Column || flex.direction == Dir::ColumnReverse;
    bool isReverse = flex.direction == Dir::RowReverse || flex.direction == Dir::ColumnReverse;

    float mainSize = isColumn ? innerRect.size.height : innerRect.size.width;
    float crossSize = isColumn ? innerRect.size.width : innerRect.size.height;

    ItemGap itemGap { flex.gap, isColumn };
    float freeSpace = this->calculateFreeSpace(item, itemGap, mainSize);

    std::vector<Entity> orderedChildren = children;
    if (isReverse) std::reverse(orderedChildren.begin(), orderedChildren.end());

    float mainPos = isColumn ? innerRect.position.y : innerRect.position.x;
    DistributionParams params { mainPos, freeSpace, flex.justify };
    this->distributeMainAxis(item, itemGap, params);
    
    float crossPos = isColumn ? innerRect.position.x : innerRect.position.y;
    AlignmentParams alignParams { crossPos, crossSize, isColumn, flex.align };
    
    for (auto child : orderedChildren)
    { this->alignItemCross({item.ctx, child}, alignParams); }
}
