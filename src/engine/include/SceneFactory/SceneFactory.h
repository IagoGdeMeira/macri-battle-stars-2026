#ifndef scene_factory_h
#define scene_factory_h

#include "../include/ISceneFactory/ISceneFactory.h"

#include <any>
#include <memory>
#include <vector>

class Engine;
class Window;
struct InputContext;
struct TriggerContext;
class CharacterRoster;
class CharacterLoader;
class Camera2D;
struct Combo;

class SceneFactory : public ISceneFactory
{
public:
    SceneFactory(
        Window& window,
        InputContext& inputContext,
        TriggerContext& triggerContext,
        CharacterRoster& characterRoster,
        CharacterLoader& characterLoader,
        Camera2D& camera,
        std::vector<Combo>& globalCombos
    );

    std::unique_ptr<Scene> createScene(SceneId id, std::any data) override;

    Engine* engine = nullptr;

private:
    Window& window;
    InputContext& inputContext;
    TriggerContext& triggerContext;
    CharacterRoster& characterRoster;
    CharacterLoader& characterLoader;
    Camera2D& camera;
    std::vector<Combo>& globalCombos;
};

#endif // scene_factory_h
