#include "AlignItemsHandler.h"

#include "FlexLayoutContext/FlexLayoutContext.h"

#include "domain/components/FlexContainer.h"
#include "domain/components/FlexItem.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/FlexEnums/FlexEnums.h"

void AlignItemsHandler::layout(FlexLayoutContext& ctx)
{
    auto& comp = ctx.world.components();
    float crossPos = ctx.isColumn ? ctx.innerRect.position.x : ctx.innerRect.position.y;
    float crossSize = ctx.isColumn ? ctx.innerRect.size.width : ctx.innerRect.size.height;

    for (auto& info : ctx.childInfos)
    {
        float marginCrossStart = ctx.isColumn ? info.margin.left : info.margin.top;
        float marginCrossEnd = ctx.isColumn ? info.margin.right : info.margin.bottom;

        AlignItems align = ctx.flex.align;
        if (comp.has<FlexItem>(info.entity))
        {
            const auto& flexItem = comp.get<FlexItem>(info.entity);
            if (flexItem.alignSelf.has_value()) align = flexItem.alignSelf.value();
        }

        Entity child = info.entity;
        if (!comp.has<LocalTransform>(child)) comp.add<LocalTransform>(child, LocalTransform{});
        auto& childLocal = comp.get<LocalTransform>(child);

        float pos = crossPos + marginCrossStart;

        if (align == AlignItems::Center) pos += (crossSize - info.crossSize - marginCrossStart - marginCrossEnd) * 0.5f;
        else if (align == AlignItems::FlexEnd) pos += crossSize - info.crossSize - marginCrossStart - marginCrossEnd;

        if (ctx.isColumn) childLocal.position.x = pos;
        else childLocal.position.y = pos;

        if (align == AlignItems::Stretch)
        {
            float finalCrossSize = crossSize - marginCrossStart - marginCrossEnd;
            auto& childLayout = comp.get<UILayoutMetricsComponent>(child);
            if (ctx.isColumn) childLayout.size.width = finalCrossSize;
            else childLayout.size.height = finalCrossSize;
        }
    }
}
