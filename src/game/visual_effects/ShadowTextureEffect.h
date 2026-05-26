#ifndef shadow_texture_effect_h
#define shadow_texture_effect_h

#include "../include/IVisualEffect/IVisualEffect.h"

#include "../../domain/components/ShadowComponent.h"

#include "../../engine/include/DrawBatch/DrawTextureBatch.h"

class ShadowTextureEffect : public IVisualEffect<DrawTextureBatch, DrawTextureCommand>
{
public:
    explicit ShadowTextureEffect(const ShadowComponent& config) : config(config) {}

    void apply(DrawTextureBatch& batch, const DrawTextureCommand& base) const override
    {
        if (!this->config.enabled) return;
        DrawTextureCommand cmd = base;
        cmd.tint = this->config.color;
        cmd.dest.position.x += this->config.offset.x;
        cmd.dest.position.y += this->config.offset.y;
        batch.add(cmd);
    }

private:
    ShadowComponent config;
};

#endif // shadow_texture_effect_h
