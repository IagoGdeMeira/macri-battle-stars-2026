#ifndef draw_font_batch_h
#define draw_font_batch_h

#include "DrawBatch.h"

#include "../DrawCommands/DrawCommands.h"
#include "../Renderer/Renderer.h"

class DrawFontBatch : public DrawBatch<DrawFontCommand>
{
public:
    void submit(Renderer& renderer) override
    {
        this->sort();
        for (const auto& cmd : this->commands) renderer.drawFont(cmd);
    }
};

#endif // draw_font_batch_h
