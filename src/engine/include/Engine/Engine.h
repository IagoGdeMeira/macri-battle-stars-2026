#ifndef engine_h
#define engine_h

#include "../EventBus/EventBus.h"
#include "../GameSettings/GameSettings.h"
#include "../InputAdapter/InputAdapter.h"
#include "../InputManager/InputManager.h"
#include "../Renderer/Renderer.h"
#include "../SceneFactory/SceneFactory.h"
#include "../SceneManager/SceneManager.h"
#include "../Window/Window.h"

class Engine
{
public:
    Engine(Window& window, SceneFactory& factory, GameSettings& settings);

    Renderer* getRenderer() { return this->renderer; } 

    void setRenderer(Renderer& r) { this->renderer = &r; }

    void run();
    void stop() { this->running = false; }

    SceneManager& scenes() { return this->sceneManager; }
    EventBus& events() { return this->eventBus; }
    InputManager& input() { return this->inputManager; }
    GameSettings& settings() { return this->gameSettings; }

private:
    bool running = true;

    Window& window;
    GameSettings& gameSettings;
    Renderer* renderer = nullptr;

    SceneFactory& sceneFactory;
    SceneManager sceneManager;
    EventBus eventBus;
    InputManager inputManager;

    void updateViewport();
};

#endif // engine_h
