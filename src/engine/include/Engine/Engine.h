#ifndef engine_h
#define engine_h

#include "../EventBus/EventBus.h"
#include "../InputAdapter/InputAdapter.h"
#include "../SceneManager/SceneManager.h"
#include "../Window/Window.h"

class Engine
{
public:
    Engine(Window& window) : window(window) {}

    void setInputAdapter(InputAdapter& i) { this->input = &i; }

    void run();
    void stop() { this->running = false; }

    SceneManager& scenes() { return this->sceneManager; }
    EventBus& events() { return this->eventBus; }

private:
    bool running = true;

    Window& window;
    InputAdapter* input = nullptr;
    
    SceneManager sceneManager;
    EventBus eventBus;
};

#endif // engine_h
