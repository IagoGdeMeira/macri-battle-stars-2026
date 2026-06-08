#include "../include/Engine/Engine.h"

#include "../events/QuitEvent.h"
#include "../include/SceneManager/SceneManager.h"

#include <chrono>
#include <stdexcept>

Engine::Engine(Window& window, GameSettings& settings) : window(window), gameSettings(settings)
{ this->eventBus.subscribe<QuitEvent>([this](const QuitEvent&) { this->stop(); }); }

SceneManager& Engine::scenes()
{
    if (!this->sceneManager) throw std::runtime_error("SceneManager not set. Call setSceneManager() first.");
    return *this->sceneManager;
}

void Engine::run()
{
    if (!this->sceneManager) throw std::runtime_error("Cannot run without SceneManager");
    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();

    while (this->running)
    {
        this->inputManager.poll();

        auto now = clock::now();
        float deltaTime = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        this->sceneManager->update(deltaTime);
        this->sceneManager->render();

        if (this->renderer) this->renderer->present();
    }
}
