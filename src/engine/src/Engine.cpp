#include "../include/Engine/Engine.h"

#include "../events/QuitEvent.h"
#include "../events/WindowResizedEvent.h"

#include <chrono>

Engine::Engine(Window& window, ISceneFactory& factory, GameSettings& settings) :
    window(window), sceneManager(factory), gameSettings(settings)
{
    this->eventBus.subscribe<QuitEvent>([this](const QuitEvent&) { this->stop(); });
    this->updateViewport();

    this->eventBus.subscribe<WindowResizedEvent>([this](const WindowResizedEvent& e)
    { this->updateViewport(); });
}

void Engine::run()
{
    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();

    while (this->running)
    {
        this->inputManager.poll();

        auto now = clock::now();
        float deltaTime = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        this->sceneManager.update(deltaTime);
        this->sceneManager.render();

        if (this->renderer) this->renderer->present();
    }
}

void Engine::updateViewport()
{
    int w, h;
    this->window.getSize(w, h);

    this->gameSettings.screen.size = { static_cast<float>(w), static_cast<float>(h) };
    this->gameSettings.viewport = { 0, 0, w, h };
}
    