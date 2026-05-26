#include "../include/UIDrawer/UIDrawer.h"

#include "../render_formats/CircleRenderFormat.h"
#include "../render_formats/FontRenderFormat.h"
#include "../render_formats/RectangleRenderFormat.h"
#include "../render_formats/TextureRenderFormat.h"

UIDrawer::UIDrawer(Renderer& renderer) : renderer(renderer)
{
    this->addFormat(std::make_unique<CircleRenderFormat>(this->renderer));
    this->addFormat(std::make_unique<FontRenderFormat>(this->renderer));
    this->addFormat(std::make_unique<RectangleRenderFormat>(this->renderer));
    this->addFormat(std::make_unique<TextureRenderFormat>(this->renderer));
}

void UIDrawer::draw(RenderContext& ctx)
{
    this->renderer.setViewport(this->uiViewport);
    for (auto& format : this->formats) format->render(ctx);
}
