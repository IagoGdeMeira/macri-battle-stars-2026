#ifndef engine_h
#define engine_h

#include "../EventBus/EventBus.h"
#include "../InputAdapter/InputAdapter.h"
#include "../InputManager/InputManager.h"
#include "../GameSettings/GameSettings.h"
#include "../Renderer/Renderer.h"
#include "../SceneFactory/SceneFactory.h"
#include "../SceneManager/SceneManager.h"
#include "../Window/Window.h"

#include <optional>

class Engine
{
public:
    Engine(Window& window, GameSettings& settings);

    Renderer* getRenderer() { return this->renderer; } 
    void setRenderer(Renderer& r) { this->renderer = &r; }

    SceneFactory* getSceneFactory() { return this->sceneFactory; }
    void setSceneFactory(SceneFactory& f) { this->sceneFactory = &f; this->sceneManager.emplace(f); }

    void run();
    void stop() { this->running = false; }

    SceneManager& scenes() { return *this->sceneManager; }
    EventBus& events() { return this->eventBus; }
    InputManager& input() { return this->inputManager; }
    GameSettings& settings() { return this->gameSettings; }

private:
    bool running = true;

    Renderer* renderer = nullptr;
    SceneFactory* sceneFactory = nullptr;

    Window& window;
    GameSettings& gameSettings;
    std::optional<SceneManager> sceneManager;
    EventBus eventBus;
    InputManager inputManager;
};

#endif // engine_h
