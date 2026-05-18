#ifndef ui_render_system
#define ui_render_system

#include "../../../domain/include/Color/Color.h"
#include "../../../domain/include/Entity/Entity.h"
#include "../../../domain/include/Geometry/Geometry.h"
#include "../../../domain/include/World/World.h"

#include "../../../engine/include/RenderContext/RenderContext.h"
#include "../../../engine/include/Renderer/Renderer.h"
#include "../../../engine/include/System/System.h"

#include <vector>
#include <cstddef>

class UIRenderSystem : public System
{
public:
    UIRenderSystem(Renderer& renderer) : renderer(renderer) {}

    void update(UpdateContext&) override {}
    void draw(RenderContext& ctx);

private:
    struct BuildParams { Entity& entity; World& world; size_t order; };

    void renderSprites(RenderContext& ctx);
    void renderTexts(RenderContext& ctx);
    
    SpriteCommand buildSpriteCommand(BuildParams params) const;

    void sortSpriteCommands(std::vector<SpriteCommand>& commands) const;
    void sortTextCommands(std::vector<TextCommand>& commands) const;
    void submitSpriteCommands(const std::vector<SpriteCommand>& commands) const;
    void submitTextCommands(const std::vector<TextCommand>& commands) const;

    Renderer& renderer;
};

#endif // ui_render_system
