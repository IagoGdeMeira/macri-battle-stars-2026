#ifndef scene_factory_h
#define scene_factory_h

#include "../include/ISceneFactory/ISceneFactory.h"

#include "../../game/include/Camera2D/Camera2D.h"
#include "../../game/include/CharacterLoader/CharacterLoader.h"
#include "../../game/include/CharacterRoster/CharacterRoster.h"
#include "../../game/include/MapRoster/MapRoster.h"

#include <any>
#include <memory>
#include <vector>

class DataParser;
class Engine;
class ResourceManager;
class Window;

struct Combo;
struct InputContext;
struct TriggerContext;

class SceneFactory : public ISceneFactory
{
public:
    struct Config
    {
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
        TextureLoader& textureLoader;
    };

    explicit SceneFactory(Config&& config) :
        window(config.window),
        inputContext(config.inputContext),
        triggerContext(config.triggerContext),
        characterRoster(config.characterRoster),
        characterLoader(config.characterLoader),
        camera(config.camera),
        globalCombos(config.globalCombos),
        mapRoster(config.mapRoster),
        parser(config.parser),
        resourceManager(config.resourceManager),
        textureLoader(config.textureLoader) {}

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
    TextureLoader& textureLoader;
};

#endif // scene_factory_h
