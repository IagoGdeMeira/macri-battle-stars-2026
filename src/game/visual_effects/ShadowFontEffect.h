#ifndef shadow_font_effect_h
#define shadow_font_effect_h

#include "IVisualEffect/IVisualEffect.h"

#include "domain/components/ShadowComponent.h"

#include "engine/draw_commands/DrawFontCommand.h"

class ShadowFontEffect : public IVisualEffect<DrawFontCommand>
{
public:
    explicit ShadowFontEffect(const ShadowComponent& config) : config(config) {}

    void apply(RenderQueue& queue, const DrawFontCommand& base) const override
    {
        if (!this->config.enabled) return;
        auto cmd = std::make_unique<DrawFontCommand>(base);
        cmd->color = this->config.color;
        cmd->dest.position.x += this->config.offset.x;
        cmd->dest.position.y += this->config.offset.y;
        queue.add(std::move(cmd));
    }

private:
    ShadowComponent config;
};

#endif // shadow_font_effect_h
