#include "../include/Engine/Engine.h"

#include "../events/QuitEvent.h"

#include <chrono>

void Engine::run()
{
    using clock = std::chrono::high_resolution_clock;

    this->eventBus.subscribe<QuitEvent>([this](const QuitEvent&) { this->stop(); });

    auto lastTime = clock::now();

    while (this->running)
    {
        auto now = clock::now();
        std::chrono::duration<float> deltaTime = now - lastTime;
        lastTime = now;

        this->sceneManager.update(deltaTime.count());
    }
}
