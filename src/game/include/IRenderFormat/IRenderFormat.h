#ifndef i_render_format_h
#define i_render_format_h

#include "engine/include/DrawCommand/DrawCommand.h"
#include "engine/include/RenderQueue/RenderQueue.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

#include <vector>

class IRenderFormat
{
public:
    virtual ~IRenderFormat() = default;
    virtual void render(RenderContext& ctx, RenderQueue& queue) = 0;
};

#endif // i_render_format_h
