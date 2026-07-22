#ifndef outline_font_effect_h
#define outline_font_effect_h

#include "IVisualEffect/IVisualEffect.h"

#include "domain/components/OutlineComponent.h"

#include "engine/draw_batches/DrawFontBatch.h"

class OutlineFontEffect : public IVisualEffect<DrawFontBatch, DrawFontCommand>
{
public:
    explicit OutlineFontEffect(const OutlineComponent& config) : config(config) {}

    void apply(DrawFontBatch& batch, const DrawFontCommand& base) const override
    {
        if (!this->config.enabled) return;

        auto& t = this->config.thickness;
        for (float dx = -t; dx <= t; dx += t) for (float dy = -t; dy <= t; dy += t)
        {
            if (dx == 0.f && dy == 0.f) continue;

            DrawFontCommand cmd = base;
            cmd.color = this->config.color;
            cmd.dest.position.x += dx;
            cmd.dest.position.y += dy;
            batch.add(cmd);
        }
    }

private:
    OutlineComponent config;
};

#endif // outline_font_effect_h
