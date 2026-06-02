#include "../include/UIDrawer/UIDrawer.h"

#include "../render_formats/UICircleRenderFormat.h"
#include "../render_formats/UIFontRenderFormat.h"
#include "../render_formats/UIRectangleRenderFormat.h"
#include "../render_formats/UITextureRenderFormat.h"

#include "../../engine/events/WindowResizedEvent.h"

UIDrawer::UIDrawer(EventBus& bus, Renderer& renderer, GameSettings& settings) :
    renderer(renderer), settings(settings)
{
    this->recalculateViewport();

    bus.subscribe<WindowResizedEvent>([this](const WindowResizedEvent&)
    { this->recalculateViewport(); });
    
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

void UIDrawer::recalculateViewport()
{
    const Dimension2D& winSize = this->settings.screen.size;
    const float scale = std::min(
        winSize.width / GameConstants::VIRTUAL_SIZE.width,
        winSize.height / GameConstants::VIRTUAL_SIZE.height);

    const Dimension2D& viewSize = { GameConstants::VIRTUAL_SIZE.width * scale, GameConstants::VIRTUAL_SIZE.height * scale };
    const Position offset = {(winSize.width - viewSize.width) * 0.5f, (winSize.height - viewSize.height) * 0.5f};

    this->uiViewport =
    {
        static_cast<int>(offset.x), static_cast<int>(offset.y),
        static_cast<int>(viewSize.width), static_cast<int>(viewSize.height)
    };
}
