#include "AlignItemsHandler.h"

#include "FlexLayoutContext/FlexLayoutContext.h"

#include "domain/components/FlexContainer.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/include/World/World.h"

void AlignItemsHandler::layout(FlexLayoutContext& ctx)
{
    auto& comp = ctx.world.components();
    float crossPos = ctx.isColumn ? ctx.innerRect.position.x : ctx.innerRect.position.y;
    float crossSize = ctx.isColumn ? ctx.innerRect.size.width : ctx.innerRect.size.height;

    using Align = FlexContainer::AlignItems;

    for (auto& info : ctx.childInfos)
    {
        float marginCrossStart = ctx.isColumn ? info.margin.left : info.margin.top;
        float marginCrossEnd = ctx.isColumn ? info.margin.right : info.margin.bottom;

        float pos = crossPos + marginCrossStart;

        if (ctx.flex.align == Align::Center) pos += (crossSize - info.crossSize - marginCrossStart - marginCrossEnd) * 0.5f;
        else if (ctx.flex.align == Align::FlexEnd) pos += crossSize - info.crossSize - marginCrossStart - marginCrossEnd;

        auto& childTransform = comp.get<TransformComponent>(info.entity);
        if (ctx.isColumn) childTransform.position.x = pos;
        else childTransform.position.y = pos;

        if (ctx.flex.align == Align::Stretch)
        {
            float finalCrossSize = crossSize - marginCrossStart - marginCrossEnd;
            auto& childLayout = comp.get<UILayoutMetricsComponent>(info.entity);
            if (ctx.isColumn) childLayout.size.width = finalCrossSize;
            else childLayout.size.height = finalCrossSize;
        }
    }
}
