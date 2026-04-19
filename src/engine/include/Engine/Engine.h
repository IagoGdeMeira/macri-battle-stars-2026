#ifndef engine_h
#define engine_h

#include "../EventBus/EventBus.h"
#include "../InputAdapter/InputAdapter.h"
#include "../ISceneFactory/ISceneFactory.h"
#include "../Renderer/Renderer.h"
#include "../SceneManager/SceneManager.h"
#include "../Window/Window.h"

class Engine
{
public:
    Engine(Window& window, ISceneFactory& factory) :
        window(window), sceneManager(factory) {}

    Renderer* getRenderer() { return this->renderer; } 

    void setInputAdapter(InputAdapter& i) { this->input = &i; }
    void setRenderer(Renderer& r) { this->renderer = &r; }

    void run();
    void stop() { this->running = false; }

    SceneManager& scenes() { return this->sceneManager; }
    EventBus& events() { return this->eventBus; }

private:
    bool running = true;

    Window& window;
    InputAdapter* input = nullptr;
    Renderer* renderer = nullptr;
    
    SceneManager sceneManager;
    EventBus eventBus;
};

#endif // engine_h
