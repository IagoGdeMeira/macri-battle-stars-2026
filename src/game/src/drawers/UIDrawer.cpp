#include "UIDrawer/UIDrawer.h"

#include "UICircleRenderFormat.h"
#include "UIFontRenderFormat.h"
#include "UIRectangleRenderFormat.h"
#include "UITextureRenderFormat.h"

#include "engine/events/WindowResizedEvent.h"

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
    float viewportScale = static_cast<float>(this->uiViewport.width) / this->vSize.width;
    float totalScale = viewportScale;

    this->renderer.setViewport(this->uiViewport);
    this->renderer.setScale(Position{totalScale, totalScale});

    RenderQueue queue;
    for (auto& format : this->formats) format->render(ctx, queue);
    queue.submit(this->renderer);

    this->renderer.setScale(Position{1.f, 1.f});
}

void UIDrawer::recalculateViewport()
{
    const Dimension2D& winSize = this->settings.screen.size;

    const float scale = std::min(winSize.width / this->vSize.width, winSize.height / this->vSize.height);
    const Dimension2D& viewSize = { this->vSize.width * scale, this->vSize.height * scale };
    const Position offset = {(winSize.width - viewSize.width) * 0.5f, (winSize.height - viewSize.height) * 0.5f};

    this->uiViewport = {
        static_cast<int>(offset.x), static_cast<int>(offset.y),
        static_cast<int>(viewSize.width), static_cast<int>(viewSize.height)
    };
}
