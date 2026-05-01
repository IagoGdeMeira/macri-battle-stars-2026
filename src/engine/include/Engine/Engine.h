#ifndef engine_h
#define engine_h

#include "../EventBus/EventBus.h"
#include "../InputAdapter/InputAdapter.h"
#include "../InputManager/InputManager.h"
#include "../ISceneFactory/ISceneFactory.h"
#include "../Renderer/Renderer.h"
#include "../SceneManager/SceneManager.h"
#include "../Window/Window.h"

class Engine
{
public:
    Engine(Window& window, ISceneFactory& factory);

    Renderer* getRenderer() { return this->renderer; } 

    void setRenderer(Renderer& r) { this->renderer = &r; }

    void run();
    void stop() { this->running = false; }

    SceneManager& scenes() { return this->sceneManager; }
    EventBus& events() { return this->eventBus; }
    InputManager& input() { return this->inputManager; }

private:
    bool running = true;
    Window& window;
    Renderer* renderer = nullptr;
    SceneManager sceneManager;
    EventBus eventBus;
    InputManager inputManager;
};

#endif // engine_h
