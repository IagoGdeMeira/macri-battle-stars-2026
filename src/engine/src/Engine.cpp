#include "../include/Engine/Engine.h"

#include "../include/SceneFactory/SceneFactory.h"
#include "../include/SceneManager/SceneManager.h"

#include "../events/QuitEvent.h"

#include <chrono>

Engine::Engine(Window& window, GameSettings& settings) : window(window), gameSettings(settings)
{ this->eventBus.subscribe<QuitEvent>([this](const QuitEvent&) { this->stop(); }); }

void Engine::setSceneFactory(SceneFactory& factory)
{
    this->sceneFactory = &factory;
    this->sceneManager.emplace(factory);
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

        if (this->sceneManager.has_value())
        {
            this->sceneManager->update(deltaTime);
            this->sceneManager->render();
        }

        if (this->renderer) this->renderer->present();
    }
}
