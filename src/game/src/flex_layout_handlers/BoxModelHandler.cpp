#include "BoxModelHandler.h"

#include "FlexLayoutContext/FlexLayoutContext.h"

#include "domain/components/BoxModel.h"
#include "domain/components/FlexContainer.h"
#include "domain/components/FlexItem.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIRectComponent.h"
#include "domain/include/World/World.h"

void BoxModelHandler::layout(FlexLayoutContext& ctx)
{
    auto& comp = ctx.world.components();
    const auto& transform = comp.get<TransformComponent>(ctx.container);
    const auto& uiRect = comp.get<UIRectComponent>(ctx.container);

    Rectangle outerRect{transform.position, uiRect.size};

    AABB padding{0, 0, 0, 0}, border{0, 0, 0, 0};
    if (comp.has<BoxModel>(ctx.container))
    {
        const auto& box = comp.get<BoxModel>(ctx.container);
        padding = box.padding;
        border = box.border;
    }

    ctx.innerRect.position.x = outerRect.position.x + border.left + padding.left;
    ctx.innerRect.position.y = outerRect.position.y + border.top + padding.top;
    ctx.innerRect.size.width = outerRect.size.width - border.left - border.right - padding.left - padding.right;
    ctx.innerRect.size.height = outerRect.size.height - border.top - border.bottom - padding.top - padding.bottom;

    ctx.isColumn = false;
    ctx.isColumn |= ctx.flex.direction == FlexContainer::FlexDirection::Column;
    ctx.isColumn |= ctx.flex.direction == FlexContainer::FlexDirection::ColumnReverse;
    
    ctx.isReverse = false;
    ctx.isReverse |= ctx.flex.direction == FlexContainer::FlexDirection::RowReverse;
    ctx.isReverse |= ctx.flex.direction == FlexContainer::FlexDirection::ColumnReverse;

    ctx.childInfos.clear();

    for (Entity child : ctx.children)
    {
        ChildLayoutInfo info{ .entity = child };

        if (comp.has<BoxModel>(child)) info.margin = comp.get<BoxModel>(child).margin;

        float basis = -1.f;
        if (comp.has<FlexItem>(child)) basis = comp.get<FlexItem>(child).basis;

        const auto& childRect = comp.get<UIRectComponent>(child);

        float currentMain = ctx.isColumn ? childRect.size.height : childRect.size.width;
        info.mainSize = (basis >= 0.f) ? basis : currentMain;
        info.crossSize = ctx.isColumn ? childRect.size.width : childRect.size.height;

        if (comp.has<FlexItem>(child))
        {
            info.grow = comp.get<FlexItem>(child).grow;
            info.shrink = comp.get<FlexItem>(child).shrink;
        }

        ctx.childInfos.push_back(info);
    }
}
