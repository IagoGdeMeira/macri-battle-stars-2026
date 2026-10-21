#ifndef stub_scene_h
#define stub_scene_h

#include "engine/include/Scene/Scene.h"

class StubScene : public Scene
{
public:
    explicit StubScene(EventBus& eventBus) : Scene(eventBus) {}

    void init() override {}
};

#endif // stub_scene_h
