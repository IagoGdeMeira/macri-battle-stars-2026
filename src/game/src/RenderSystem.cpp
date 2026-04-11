#include "../include/RenderSystem/RenderSystem.h"

#include "../../domain/components/RenderComponent.h"
#include "../../domain/components/SpriteComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/events/WindowResizedEvent.h"
#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <algorithm>

RenderSystem::RenderSystem(EventBus& bus, Renderer& renderer, Camera2D& camera) :
    renderer(renderer), camera(camera)
{
    this->updateViewports();

    bus.subscribe<WindowResizedEvent>([this](const WindowResizedEvent& event)
    {
        this->windowWidth = event.width;
        this->windowHeight = event.height;
        this->updateViewports();
    });
}

void RenderSystem::update(UpdateContext& ctx)
{
    this->renderer.clear();

    this->renderer.setViewport(this->worldViewport);
    this->renderWorld(ctx);

    this->renderer.setViewport(this->uiViewport);
    this->renderUI(ctx);

    this->renderer.present();
}

void RenderSystem::renderWorld(UpdateContext& ctx)
{
    std::vector<DrawCommand> commands;
    commands.reserve(128);

    auto view = View<TransformComponent, SpriteComponent, RenderComponent>(
        ctx.world.components());

    size_t orderCounter = 0;

    for (auto [entity, transform, sprite, render] : view)
    {
        if (!sprite.texture) continue;

        float screenX, screenY;
        this->worldToScreen(
            transform.x, transform.y,
            screenX, screenY,
            this->worldViewport);

        int finalWidth = static_cast<int>(
            sprite.width * transform.scaleX * camera.getZoom());

        int finalHeight = static_cast<int>(
            sprite.height * transform.scaleY * camera.getZoom());

        commands.push_back({
            sprite.texture.get(),
            static_cast<int>(screenX),
            static_cast<int>(screenY),
            finalWidth,
            finalHeight,
            render.layer,
            render.zIndex,
            orderCounter++
        });
    }

    std::stable_sort(commands.begin(), commands.end(),
        [](const DrawCommand& a, const DrawCommand& b)
        {
            if (a.layer != b.layer) return a.layer < b.layer;
            if (a.zIndex != b.zIndex) return a.zIndex < b.zIndex;
            return a.order < b.order;
        });

    for (const auto& cmd : commands) this->renderer.draw(
        *cmd.texture,
        cmd.x, cmd.y,
        cmd.width, cmd.height);
}

void RenderSystem::renderUI(UpdateContext& ctx)
{
    (void)ctx;
    // TODO: Implement UI rendering
    // This function will handle rendering UI elements on top of the game world
    // Something like: View<UISpriteComponent, UITransformComponent, RenderComponent>
};

void RenderSystem::worldToScreen(
    float worldX, float worldY,
    float& screenX, float& screenY,
    const Viewport& viewport
) {
    float camX = this->camera.getX();
    float camY = this->camera.getY();
    float zoom = this->camera.getZoom();

    screenX = (worldX - camX) * zoom + (viewport.width * 0.5f);
    screenY = (worldY - camY) * zoom + (viewport.height * 0.5f);
}

void RenderSystem::updateViewports()
{
    float scaleX = (float)this->windowWidth / this->VIRTUAL_WIDTH;
    float scaleY = (float)this->windowHeight / this->VIRTUAL_HEIGHT;

    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    int viewWidth = (int)(this->VIRTUAL_WIDTH * scale);
    int viewHeight = (int)(this->VIRTUAL_HEIGHT * scale);

    int offsetX = (this->windowWidth - viewWidth) / 2;
    int offsetY = (this->windowHeight - viewHeight) / 2;

    this->worldViewport = { offsetX, offsetY, viewWidth, viewHeight};
    this->uiViewport = { 0, 0, this->windowWidth, this->windowHeight};
}
