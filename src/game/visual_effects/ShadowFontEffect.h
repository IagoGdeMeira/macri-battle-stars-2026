#ifndef shadow_font_effect_h
#define shadow_font_effect_h

#include "IVisualEffect/IVisualEffect.h"

#include "domain/components/ShadowComponent.h"

#include "engine/include/DrawBatch/DrawFontBatch.h"

class ShadowFontEffect : public IVisualEffect<DrawFontBatch, DrawFontCommand>
{
public:
    explicit ShadowFontEffect(const ShadowComponent& config) : config(config) {}

    void apply(DrawFontBatch& batch, const DrawFontCommand& base) const override
    {
        if (!this->config.enabled) return;
        DrawFontCommand cmd = base;
        cmd.color = this->config.color;
        cmd.dest.position.x += this->config.offset.x;
        cmd.dest.position.y += this->config.offset.y;
        batch.add(cmd);
    }

private:
    ShadowComponent config;
};

#endif // shadow_font_effect_h
