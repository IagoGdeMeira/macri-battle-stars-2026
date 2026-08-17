#include "MainAxisHandler.h"

#include "FlexLayoutContext/FlexLayoutContext.h"

#include "domain/components/FlexContainer.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/include/World/World.h"

void MainAxisHandler::layout(FlexLayoutContext& ctx)
{
    auto& comp = ctx.world.components();
    float mainPos = (ctx.isColumn ? ctx.innerRect.position.y : ctx.innerRect.position.x) + ctx.startOffset;

    for (size_t i = 0; i < ctx.childInfos.size(); ++i)
    {
        auto& info = ctx.childInfos[i];
        float marginStart = ctx.isColumn ? info.margin.top : info.margin.left;
        float marginEnd = ctx.isColumn ? info.margin.bottom : info.margin.right;

        auto& childTransform = comp.get<TransformComponent>(info.entity);
        if (ctx.isColumn) childTransform.position.y = mainPos + marginStart;
        else childTransform.position.x = mainPos + marginStart;

        auto& childLayout = comp.get<UILayoutMetricsComponent>(info.entity);
        if (ctx.isColumn) childLayout.size.height = info.mainSize;
        else childLayout.size.width = info.mainSize;

        mainPos += info.mainSize + marginStart + marginEnd;
        if (i != ctx.childInfos.size() - 1) mainPos += ctx.flex.gap + ctx.between;
    }
}
