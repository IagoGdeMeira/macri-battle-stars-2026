#ifndef stub_scene_manager_h
#define stub_scene_manager_h

#include "StubEngine.h"
#include "StubSceneFactory.h"
#include "StubWindow.h"

#include "engine/include/SceneManager/SceneManager.h"
#include "engine/value_objects/GameSettings/GameSettings.h"

#include <vector>

class StubSceneManager : public SceneManager
{
public:
    int updateCalls = 0;
    std::vector<float> deltas;
    
    StubSceneManager() : SceneManager(this->factory, this->engine) {}
    StubSceneManager(SceneFactory& factory, Engine& engine) : SceneManager(factory, engine) {}

    void update(float deltaTime) override
    {
        this->updateCalls++;
        this->deltas.push_back(deltaTime);
        SceneManager::update(deltaTime);
    }

private:
    StubSceneFactory factory;
    StubEngine engine{this->dummyWindow, this->dummySettings};
    StubWindow dummyWindow;
    GameSettings dummySettings;
};

#endif // stub_scene_manager_h
