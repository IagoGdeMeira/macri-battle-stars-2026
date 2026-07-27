#include "Engine/Engine.h"

#include "QuitEvent.h"
#include "SceneManager/SceneManager.h"

#include "domain/utils/Logger/Logger.h"

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

    const int TARGET_FPS = this->gameSettings.targetFPS;
    const float FIXED_DELTA = 1.f / static_cast<float>(TARGET_FPS);
    const int MAX_UPDATES_PER_FRAME = 5;

    using hrclock = std::chrono::high_resolution_clock;
    using duration = std::chrono::duration<float>;
    using ms = std::chrono::milliseconds;

    float accumulator = 0.f;
    auto previousTime = hrclock::now();

    while (this->running)
    {
        auto t0 = hrclock::now();

        this->inputManager.poll();
        auto t1 = hrclock::now();

        auto currentTime = hrclock::now();
        float deltaTime = duration(currentTime - previousTime).count();
        previousTime = currentTime;

        if (deltaTime > 0.1f) deltaTime = 0.1f;

        accumulator += deltaTime;
        auto t2 = hrclock::now();

        int updateCount = 0;
        while (accumulator >= FIXED_DELTA && updateCount < MAX_UPDATES_PER_FRAME)
        {
            this->sceneManager->update(FIXED_DELTA);
            accumulator -= FIXED_DELTA;
            ++updateCount;
        }
        auto t3 = hrclock::now();

        this->sceneManager->render();
        auto t4 = hrclock::now();

        if (this->renderer) this->renderer->present();
        auto t5 = hrclock::now();

        std::this_thread::yield();
        auto t6 = hrclock::now();

        auto log_duration = [](auto start, auto end, const char* label)
        {
            auto msCount = std::chrono::duration_cast<ms>(end - start).count();
            if (msCount > 5) LOG_DEBUG("{} took {} ms", label, msCount);
        };

        log_duration(t0, t1, "input.poll");
        log_duration(t1, t2, "delta calc");
        log_duration(t2, t3, "update");
        log_duration(t3, t4, "render");
        log_duration(t4, t5, "present");
        log_duration(t5, t6, "yield");
    }
}
