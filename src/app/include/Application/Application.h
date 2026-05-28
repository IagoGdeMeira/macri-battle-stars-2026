#ifndef application_h
#define application_h

#include "../../app/include/ISystemInitializer/ISystemInitializer.h"

#include "../../engine/include/Engine/Engine.h"
#include "../../engine/include/GameSettings/GameSettings.h"
#include "../../engine/include/InputContext/InputContext.h"
#include "../../engine/include/IPlatformFactory/IPlatformFactory.h"
#include "../../engine/include/SceneFactory/SceneFactory.h"

#include "../../game/include/Camera2D/Camera2D.h"
#include "../../game/include/CharacterLoader/CharacterLoader.h"
#include "../../game/include/CharacterRoster/CharacterRoster.h"
#include "../../game/include/Combo/Combo.h"
#include "../../game/include/MapRoster/MapRoster.h"
#include "../../game/include/TriggerContext/TriggerContext.h"

#include "../../platform/include/JsonParser/JsonParser.h"

#include <memory>
#include <string>
#include <vector>

class InputAdapter;
class Renderer;
class ResourceManager;
class ThreadPool;
class Window;

class AnimationLoader;
class CharacterDefinitionLoader;
class CollisionClipLoader;
class MapLoader;
class StateMachineLoader;
class TextureLoader;

class Application
{
public:
    Application() = default;
    ~Application() = default;
    
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    Application& setWindowTitle(const std::string& title);
    Application& setWindowSize(int width, int height);

    int run();

private:
    std::string windowTitle = "Macri Battle Stars";
    int windowWidth = 800;
    int windowHeight = 600;
    GameSettings gameSettings;

    std::unique_ptr<ISystemInitializer> initializer;
    std::unique_ptr<IPlatformFactory> platformFactory;
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<SceneFactory> sceneFactory;
    std::unique_ptr<Engine> engine;
    std::unique_ptr<JsonParser> parser;
    std::unique_ptr<ThreadPool> threadPool;

    std::unique_ptr<CharacterDefinitionLoader> charDefLoader;
    std::unique_ptr<AnimationLoader> animLoader;
    std::unique_ptr<StateMachineLoader> fsmLoader;
    std::unique_ptr<TextureLoader> textureLoader;
    std::unique_ptr<CollisionClipLoader> clipLoader;

    std::unique_ptr<ResourceManager> resourceManager;
    std::unique_ptr<CharacterLoader> characterLoader;
    std::unique_ptr<CharacterRoster> characterRoster;
    std::unique_ptr<MapRoster> mapRoster;
    std::unique_ptr<Camera2D> camera;

    std::unique_ptr<InputContext> inputContext;
    std::unique_ptr<TriggerContext> triggerContext;
    std::vector<Combo>  globalCombos;

    void initSystems();
    void initLoaders();
    void loadGameData();
    void setupInput();
    void setupInitialScene();
};

#endif // application_h
