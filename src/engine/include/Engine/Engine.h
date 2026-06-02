#ifndef engine_h
#define engine_h

#include "../EventBus/EventBus.h"
#include "../InputAdapter/InputAdapter.h"
#include "../InputManager/InputManager.h"
#include "../Renderer/Renderer.h"
#include "../SceneManager/SceneManager.h"
#include "../Window/Window.h"
#include "../GameSettings/GameSettings.h"

#include <optional>

class SceneFactory;

class Engine
{
public:
    Engine(Window& window, GameSettings& settings);
    ~Engine() = default;

    Renderer* getRenderer() { return this->renderer; }
    void setRenderer(Renderer& r) { this->renderer = &r; }

    void run();
    void stop() { this->running = false; }

    SceneManager& scenes() { return *this->sceneManager; }
    EventBus& events() { return this->eventBus; }
    InputManager& input() { return this->inputManager; }
    GameSettings& settings() { return this->gameSettings; }

    void setSceneFactory(SceneFactory& factory);

private:
    bool running = true;
    Window& window;
    GameSettings& gameSettings;

    Renderer* renderer = nullptr;
    SceneFactory* sceneFactory = nullptr;

    EventBus eventBus;
    InputManager inputManager;

    std::optional<SceneManager> sceneManager;
};

#endif // engine_h
