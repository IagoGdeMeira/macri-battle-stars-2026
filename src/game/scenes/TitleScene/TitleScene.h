#ifndef title_scene_h
#define title_scene_h

#include "../../include/CharacterRoster/CharacterRoster.h"

#include "../../../engine/include/InputContext/InputContext.h"
#include "../../../engine/include/Renderer/Renderer.h"
#include "../../../engine/include/Scene/Scene.h"
#include "../../../engine/include/SceneManager/SceneManager.h"

class TitleScene : public Scene
{
public:
    struct Config
    {
        EventBus& eventBus;
        SceneManager& sceneManager;
        CharacterRoster& roster;
        InputContext& input;
        Renderer& renderer;
    };

    explicit TitleScene(Config config);
    void update(float deltaTime) override;

private:
    SceneManager& sceneManager;
    CharacterRoster& roster;
    InputContext& input;
    Renderer& renderer;
};

#endif // title_scene_h
