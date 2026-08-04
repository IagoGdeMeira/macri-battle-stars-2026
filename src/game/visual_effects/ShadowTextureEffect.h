#ifndef shadow_texture_effect_h
#define shadow_texture_effect_h

#include "IVisualEffect/IVisualEffect.h"

#include "domain/components/ShadowComponent.h"

#include "engine/draw_commands/DrawTextureCommand.h"

class ShadowTextureEffect : public IVisualEffect<DrawTextureCommand>
{
public:
    explicit ShadowTextureEffect(const ShadowComponent& config) : config(config) {}

    void apply(RenderQueue& queue, const DrawTextureCommand& base) const override
    {
        if (!this->config.enabled) return;
        auto cmd = std::make_unique<DrawTextureCommand>(base);
        cmd->tint = this->config.color;
        cmd->dest.position.x += this->config.offset.x;
        cmd->dest.position.y += this->config.offset.y;
        queue.add(std::move(cmd));
    }

private:
    ShadowComponent config;
};

#endif // shadow_texture_effect_h
