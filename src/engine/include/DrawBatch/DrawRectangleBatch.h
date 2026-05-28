#ifndef draw_rectangle_batch_h
#define draw_rectangle_batch_h

#include "DrawBatch.h"

#include "../DrawCommands/DrawCommands.h"
#include "../Renderer/Renderer.h"

class DrawRectangleBatch : public DrawBatch<DrawRectangleCommand>
{
public:
    void submit(Renderer& renderer) override
    {
        this->sort();
        for (const auto& cmd : this->commands) renderer.drawRectangle(cmd);
    }
};

#endif // draw_rectangle_batch_h
