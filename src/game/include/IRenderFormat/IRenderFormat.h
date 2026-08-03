#ifndef i_render_format_h
#define i_render_format_h

#include "engine/include/DrawCommand/DrawCommand.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

#include <vector>

class IRenderFormat
{
public:
    virtual ~IRenderFormat() = default;
    virtual void render(RenderContext& ctx) = 0;

    virtual std::vector<const DrawCommand*> collectCommands() const = 0;
};

#endif // i_render_format_h
