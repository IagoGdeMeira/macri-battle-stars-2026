#include "JustifyContentHandler.h"

#include "FlexLayoutContext/FlexLayoutContext.h"

#include "domain/components/FlexContainer.h"

void JustifyContentHandler::layout(FlexLayoutContext& ctx)
{
    ctx.startOffset = 0.f;
    ctx.between = 0.f;

    using Justify = FlexContainer::JustifyContent;
    switch (ctx.flex.justify)
    {
        case Justify::FlexEnd: ctx.startOffset = ctx.freeSpace; break;
        case Justify::Center: ctx.startOffset = ctx.freeSpace * 0.5f; break;
        case Justify::SpaceBetween:
            if (ctx.childInfos.size() > 1) ctx.between = ctx.freeSpace / static_cast<float>(ctx.childInfos.size() - 1);
            break;
        case Justify::SpaceAround:
            if (!ctx.childInfos.empty())
            {
                ctx.between = ctx.freeSpace / ctx.childInfos.size();
                ctx.startOffset = ctx.between * 0.5f;
            }
            break;
        case Justify::SpaceEvenly:
            if (!ctx.childInfos.empty())
            {
                ctx.between = ctx.freeSpace / (ctx.childInfos.size() + 1);
                ctx.startOffset = ctx.between;
            }
            break;
        case Justify::FlexStart: break;
    }
}
