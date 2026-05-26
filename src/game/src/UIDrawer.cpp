#include "../include/UIDrawer/UIDrawer.h"

#include "../render_formats/UICircleRenderFormat.h"
#include "../render_formats/UIFontRenderFormat.h"
#include "../render_formats/UIRectangleRenderFormat.h"
#include "../render_formats/UITextureRenderFormat.h"

UIDrawer::UIDrawer(Renderer& renderer) : renderer(renderer)
{
    this->addFormat(std::make_unique<UICircleRenderFormat>(this->renderer));
    this->addFormat(std::make_unique<UIFontRenderFormat>(this->renderer));
    this->addFormat(std::make_unique<UIRectangleRenderFormat>(this->renderer));
    this->addFormat(std::make_unique<UITextureRenderFormat>(this->renderer));
}

void UIDrawer::draw(RenderContext& ctx)
{
    this->renderer.setViewport(this->uiViewport);
    for (auto& format : this->formats) format->render(ctx);
}
