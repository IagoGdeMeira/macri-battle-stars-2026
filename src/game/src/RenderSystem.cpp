#include "../include/RenderSystem/RenderSystem.h"

#include "../../engine/events/WindowResizedEvent.h"

RenderSystem::RenderSystem(EventBus& bus, Renderer& renderer) : renderer(renderer)
{
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
    this->renderer.present();
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
