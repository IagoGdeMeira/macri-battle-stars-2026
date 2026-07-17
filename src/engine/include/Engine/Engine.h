#ifndef engine_h
#define engine_h

#include "../EventBus/EventBus.h"
#include "../InputManager/InputManager.h"
#include "../Renderer/Renderer.h"
#include "../Window/Window.h"

#include "../../value_objects/GameSettings/GameSettings.h"

class SceneManager;

class Engine
{
public:
    Engine(Window& window, GameSettings& settings);
    ~Engine() = default;

    void setSceneManager(SceneManager& manager) { this->sceneManager = &manager; }
    void setRenderer(Renderer& r) { this->renderer = &r; }

    void run();
    void stop() { this->running = false; }

    SceneManager& scenes();
    EventBus& events() { return this->eventBus; }
    InputManager& input() { return this->inputManager; }
    GameSettings& settings() { return this->gameSettings; }

private:
    bool running = true;
    EventBus eventBus;
    InputManager inputManager;

    Window& window;
    GameSettings& gameSettings;

    Renderer* renderer = nullptr;
    SceneManager* sceneManager = nullptr;
};

#endif // engine_h
