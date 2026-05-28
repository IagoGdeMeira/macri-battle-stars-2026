#include "../include/WorldDrawer/WorldDrawer.h"

#include "../render_formats/WorldCircleRenderFormat.h"
#include "../render_formats/WorldRectangleRenderFormat.h"
#include "../render_formats/WorldTextureRenderFormat.h"

#include "../../engine/events/WindowResizedEvent.h"

WorldDrawer::WorldDrawer(EventBus& bus, Renderer& renderer, Camera2D& camera, GameSettings& settings) :
    renderer(renderer), camera(camera), settings(settings)
{
    this->recalculateViewport();

    bus.subscribe<WindowResizedEvent>([this](const WindowResizedEvent&)
    {
        this->recalculateViewport();
        this->propagateViewport();
    });

    this->addFormat(std::make_unique<WorldTextureRenderFormat>(renderer, camera));
    this->addFormat(std::make_unique<WorldRectangleRenderFormat>(renderer, camera));
    this->addFormat(std::make_unique<WorldCircleRenderFormat>(renderer, camera));

    this->propagateViewport();
}

void WorldDrawer::draw(RenderContext& ctx)
{
    this->renderer.setViewport(this->worldViewport);
    for (auto& format : this->formats) format->render(ctx);
}

void WorldDrawer::recalculateViewport()
{
    const Dimension2D& winSize = this->settings.screen.size;
    const float scale = std::min(
        winSize.width / GameSettings::VIRTUAL_SIZE.width,
        winSize.height / GameSettings::VIRTUAL_SIZE.height);

    const Dimension2D& viewSize = { GameSettings::VIRTUAL_SIZE.width * scale, GameSettings::VIRTUAL_SIZE.height * scale };
    const Position offset = {(winSize.width - viewSize.width) * 0.5f, (winSize.height - viewSize.height) * 0.5f};

    this->worldViewport =
    {
        static_cast<int>(offset.x), static_cast<int>(offset.y),
        static_cast<int>(viewSize.width), static_cast<int>(viewSize.height)
    };
}

void WorldDrawer::propagateViewport()
{ for (auto& format : this->formats) format->setViewport(this->worldViewport); }
