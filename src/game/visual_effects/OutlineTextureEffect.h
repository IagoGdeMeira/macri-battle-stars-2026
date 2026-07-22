#ifndef outline_texture_effect_h
#define outline_texture_effect_h

#include "IVisualEffect/IVisualEffect.h"

#include "domain/components/OutlineComponent.h"

#include "engine/draw_batches/DrawTextureBatch.h"

class OutlineTextureEffect : public IVisualEffect<DrawTextureBatch, DrawTextureCommand>
{
public:
    explicit OutlineTextureEffect(const OutlineComponent& config) : config(config) {}

    void apply(DrawTextureBatch& batch, const DrawTextureCommand& base) const override
    {
        if (!this->config.enabled) return;

        auto& t = this->config.thickness;
        for (float dx = -t; dx <= t; dx += t) for (float dy = -t; dy <= t; dy += t)
        {
            if (dx == 0.f && dy == 0.f) continue;

            DrawTextureCommand cmd = base;
            cmd.tint = this->config.color;
            cmd.dest.position.x += dx;
            cmd.dest.position.y += dy;
            batch.add(cmd);
        }
    }

private:
    OutlineComponent config;
};

#endif // outline_texture_effect_h
