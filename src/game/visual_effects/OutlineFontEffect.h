#ifndef outline_font_effect_h
#define outline_font_effect_h

#include "IVisualEffect/IVisualEffect.h"

#include "domain/components/OutlineComponent.h"

#include "engine/draw_commands/DrawFontCommand.h"

class OutlineFontEffect : public IVisualEffect<DrawFontCommand>
{
public:
    explicit OutlineFontEffect(const OutlineComponent& config) : config(config) {}

    void apply(RenderQueue& queue, const DrawFontCommand& base) const override
    {
        if (!this->config.enabled) return;
        auto& t = this->config.thickness;

        for (float dx = -t; dx <= t; dx += t) for (float dy = -t; dy <= t; dy += t)
        {
            if (dx == 0.f && dy == 0.f) continue;
            auto cmd = std::make_unique<DrawFontCommand>(base);
            cmd->color = this->config.color;
            cmd->dest.position.x += dx;
            cmd->dest.position.y += dy;
            queue.add(std::move(cmd));
        }
    }

private:
    OutlineComponent config;
};

#endif // outline_font_effect_h
