#ifndef stub_scene_manager_h
#define stub_scene_manager_h

#include "StubEngine.h"
#include "StubSceneFactory.h"
#include "StubWindow.h"

#include "../../src/engine/include/GameSettings/GameSettings.h"
#include "../../src/engine/include/SceneManager/SceneManager.h"

class StubSceneManager : public SceneManager
{
public:
    StubSceneManager() : SceneManager(this->factory, this->engine) {}

private:
    StubSceneFactory factory;
    StubEngine engine{this->dummyWindow, this->dummySettings};
    StubWindow dummyWindow;
    GameSettings dummySettings;
};

#endif // stub_scene_manager_h
