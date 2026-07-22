#ifndef draw_texture_batch_h
#define draw_texture_batch_h

#include "DrawBatch/DrawBatch.h"
#include "DrawTextureCommand.h"
#include "Renderer/Renderer.h"

class DrawTextureBatch : public DrawBatch<DrawTextureCommand>
{
public:
    void submit(Renderer& renderer) override
    {
        this->sort();
        for (const auto& cmd : this->commands) renderer.draw(cmd);
    }
};

#endif // draw_texture_batch_h
