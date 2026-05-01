#include "../include/Engine/Engine.h"

#include "../events/QuitEvent.h"

#include <chrono>

Engine::Engine(Window& window, ISceneFactory& factory) : window(window), sceneManager(factory)
{ this->eventBus.subscribe<QuitEvent>([this](const QuitEvent&) { this->stop(); }); }

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
    