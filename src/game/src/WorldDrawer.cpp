#include "../include/WorldDrawer/WorldDrawer.h"

#include "../render_formats/WorldCircleRenderFormat.h"
#include "../render_formats/WorldRectangleRenderFormat.h"
#include "../render_formats/WorldTextureRenderFormat.h"

#include "../../engine/events/WindowResizedEvent.h"

WorldDrawer::WorldDrawer(EventBus& bus, Renderer& renderer, Camera2D& camera) : renderer(renderer)
{
    bus.subscribe<WindowResizedEvent>([this](const WindowResizedEvent& e)
    {
        this->windowSize = {static_cast<float>(e.width), static_cast<float>(e.height)};
        this->updateViewports();
    });

    this->addFormat(std::make_unique<WorldTextureRenderFormat>(renderer, camera));
    this->addFormat(std::make_unique<WorldRectangleRenderFormat>(renderer, camera));
    this->addFormat(std::make_unique<WorldCircleRenderFormat>(renderer, camera));

    this->updateViewports();
}

void WorldDrawer::draw(RenderContext& ctx)
{
    this->renderer.setViewport(this->worldViewport);
    for (auto& format : this->formats) format->render(ctx);
}

void WorldDrawer::updateViewports()
{
    const float scale = std::min(
        this->windowSize.width / WorldDrawer::VIRTUAL_SIZE.width,
        this->windowSize.height / WorldDrawer::VIRTUAL_SIZE.height);

    const Dimension2D scaledSize { WorldDrawer::VIRTUAL_SIZE.width * scale, WorldDrawer::VIRTUAL_SIZE.height * scale };

    Position offset
    {
        (this->windowSize.width - scaledSize.width) * 0.5f,
        (this->windowSize.height - scaledSize.height) * 0.5f
    };

    this->worldViewport =
    {
        static_cast<int>(offset.x), static_cast<int>(offset.y),
        static_cast<int>(scaledSize.width), static_cast<int>(scaledSize.height)
    };

    for (auto& format : this->formats)
    {
        if (auto* textFmt = dynamic_cast<WorldTextureRenderFormat*>(format.get()))
        { textFmt->setViewport(this->worldViewport); }
        else if (auto* rectFmt = dynamic_cast<WorldRectangleRenderFormat*>(format.get()))
        { rectFmt->setViewport(this->worldViewport); }
        else if (auto* circleFmt = dynamic_cast<WorldCircleRenderFormat*>(format.get()))
        { circleFmt->setViewport(this->worldViewport); }
    }
}
