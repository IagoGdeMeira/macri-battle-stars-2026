#include "../include/RenderSystem/RenderSystem.h"

#include "../../domain/components/SpriteComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/events/WindowResizedEvent.h"
#include "../../engine/include/UpdateContext/UpdateContext.h"

RenderSystem::RenderSystem(EventBus& bus, Renderer& renderer, Camera2D& camera) :
    renderer(renderer), camera(camera)
{
    this->updateViewport();

    bus.subscribe<WindowResizedEvent>([this](const WindowResizedEvent& event)
    {
        this->windowWidth = event.width;
        this->windowHeight = event.height;
        this->updateViewport();
    });
}

void RenderSystem::update(UpdateContext& ctx)
{
    this->renderer.clear();

    auto view = View<TransformComponent, SpriteComponent>(ctx.world.components());

    for (auto [entity, transform, sprite] : view)
    {
        float screenX, screenY;
        this->worldToScreen(transform.x, transform.y, screenX, screenY);

        int width = static_cast<int>(sprite.width * transform.scaleX * camera.getZoom());
        int height = static_cast<int>(sprite.height * transform.scaleY * camera.getZoom());

        if (sprite.texture)
        {
            this->renderer.draw(
                *sprite.texture,
                static_cast<int>(screenX),
                static_cast<int>(screenY),
                width,
                height
            );
        }
    }

    this->renderer.present();
}

void RenderSystem::worldToScreen(float worldX, float worldY, float& screenX, float& screenY)
{
    float camX = this->camera.getX();
    float camY = this->camera.getY();
    float zoom = this->camera.getZoom();

    screenX = (worldX - camX) * zoom + (this->VIRTUAL_WIDTH * 0.5f);
    screenY = (worldY - camY) * zoom + (this->VIRTUAL_HEIGHT * 0.5f);
}

void RenderSystem::updateViewport()
{
    float scaleX = (float)this->windowWidth / this->VIRTUAL_WIDTH;
    float scaleY = (float)this->windowHeight / this->VIRTUAL_HEIGHT;

    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    int viewWidth = (int)(this->VIRTUAL_WIDTH * scale);
    int viewHeight = (int)(this->VIRTUAL_HEIGHT * scale);

    int offsetX = (this->windowWidth - viewWidth) / 2;
    int offsetY = (this->windowHeight - viewHeight) / 2;

    this->renderer.setViewport(offsetX, offsetY, viewWidth, viewHeight);
}
