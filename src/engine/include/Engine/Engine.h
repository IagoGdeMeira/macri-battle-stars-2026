#ifndef engine_h
#define engine_h

#include "../EventBus/EventBus.h"
#include "../SceneManager/SceneManager.h"

class Engine
{
public:
    void run();
    void stop() { this->running = false; }

    SceneManager& scenes() { return this->sceneManager; }
    EventBus& events() { return this->eventBus; }

private:
    bool running = true;

    SceneManager sceneManager;
    EventBus eventBus;
};

#endif // engine_h
