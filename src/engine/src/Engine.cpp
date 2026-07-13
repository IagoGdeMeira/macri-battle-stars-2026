#include "../include/Engine/Engine.h"

#include "../events/QuitEvent.h"
#include "../include/SceneManager/SceneManager.h"

#include "../../domain/utils/Logger/Logger.h"

#include <chrono>
#include <stdexcept>
#include <thread>

Engine::Engine(Window& window, GameSettings& settings) : window(window), gameSettings(settings)
{
    this->eventBus.subscribe<QuitEvent>([this](const QuitEvent&)
    { this->stop(); });
}

SceneManager& Engine::scenes()
{
    if (!this->sceneManager) throw std::runtime_error("SceneManager not set. Call setSceneManager() first.");
    return *this->sceneManager;
}

void Engine::run()
{
    if (!this->sceneManager) throw std::runtime_error("Cannot run without SceneManager");

    const int targetFPS = this->gameSettings.targetFPS;
    const float fixedDelta = 1.f / static_cast<float>(targetFPS);
    const float targetFrameTime = fixedDelta;

    using hrclock = std::chrono::high_resolution_clock;
    using duration = std::chrono::duration<float>;

    LOG_DEBUG("Engine: targetFPS = {}, fixedDelta = {}", targetFPS, fixedDelta);

    float accumulator = 0.f;
    auto previousTime = hrclock::now();

    while (this->running)
    {
        auto frameStart = hrclock::now();

        this->inputManager.poll();

        auto currentTime = hrclock::now();
        float deltaTime = duration(currentTime - previousTime).count();
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        previousTime = currentTime;

        accumulator += deltaTime;
        while (accumulator >= fixedDelta)
        {
            this->sceneManager->update(fixedDelta);
            accumulator -= fixedDelta;
        }

        this->sceneManager->render();
        if (this->renderer) this->renderer->present();

        auto frameEnd = hrclock::now();
        float elapsed = duration(frameEnd - frameStart).count();
        float sleepTime = targetFrameTime - elapsed;

        if (sleepTime > 0.001f) std::this_thread::sleep_for(std::chrono::milliseconds(
            static_cast<int>(sleepTime * 1000)));
        else std::this_thread::yield();
    }
}
