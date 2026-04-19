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
        if (this->input) this->input->poll();

        auto now = clock::now();
        float deltaTime = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        this->sceneManager.update(deltaTime);
        this->sceneManager.render();

        if (this->renderer) this->renderer->present();
    }
}
