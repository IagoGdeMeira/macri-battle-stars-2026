#ifndef scene_factory_h
#define scene_factory_h

#include "../include/ISceneFactory/ISceneFactory.h"

#include <any>
#include <memory>
#include <vector>

class Camera2D;
class CharacterLoader;
class CharacterRoster;
class DataParser;
class Engine;
class MapRoster;
class ResourceManager;
class Window;

struct Combo;
struct InputContext;
struct TriggerContext;

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
        std::vector<Combo>& globalCombos,
        MapRoster& mapRoster,
        DataParser& parser,
        ResourceManager& resourceManager
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
    MapRoster& mapRoster;
    DataParser& parser;
    ResourceManager& resourceManager;
};

#endif // scene_factory_h
