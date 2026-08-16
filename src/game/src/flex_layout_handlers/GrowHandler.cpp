#include "GrowHandler.h"

#include "FlexLayoutContext/FlexLayoutContext.h"

#include "domain/components/FlexContainer.h"

void GrowHandler::layout(FlexLayoutContext& ctx)
{
    float totalMain = 0.f, totalGrow = 0.f, totalShrink = 0.f;
    for (const auto& info : ctx.childInfos)
    {
        float marginMain = ctx.isColumn
            ? (info.margin.top + info.margin.bottom)
            : (info.margin.left + info.margin.right);
        
        totalMain += info.mainSize + marginMain;
        totalGrow += info.grow;
        totalShrink += info.shrink;
    }

    float mainSize = ctx.isColumn ? ctx.innerRect.size.height : ctx.innerRect.size.width;
    float gapTotal = ctx.flex.gap * static_cast<float>(ctx.childInfos.size() - 1);
    ctx.freeSpace = mainSize - totalMain - gapTotal;

    if (ctx.freeSpace > 0.f && totalGrow > 0.f)
    {
        for (auto& info : ctx.childInfos) info.mainSize += ctx.freeSpace * (info.grow / totalGrow);
        ctx.freeSpace = 0.f;
    }
}
