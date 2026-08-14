#ifndef game_scene_h
#define game_scene_h

#include "CharacterLoader/CharacterLoader.h"
#include "Combo/Combo.h"
#include "EntityFactory/EntityFactory.h"
#include "StateMachineRegistry/StateMachineRegistry.h"
#include "TriggerContext/TriggerContext.h"
#include "WorldDrawer/WorldDrawer.h"

#include "domain/components/MapComponent.h"
#include "domain/include/Entity/Entity.h"

#include "engine/include/DataParser/DataParser.h"
#include "engine/include/Engine/Engine.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/include/IFontFactory/IFontFactory.h"
#include "engine/include/IPlatformFactory/IPlatformFactory.h"
#include "engine/include/ITextureFactory/ITextureFactory.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/Scene/Scene.h"
#include "engine/include/SceneManager/SceneManager.h"
#include "engine/include/TextureLoader/TextureLoader.h"
#include "engine/include/Window/Window.h"
#include "engine/value_objects/Camera2D/Camera2D.h"
#include "engine/value_objects/GameSettings/GameSettings.h"
#include "engine/value_objects/InputContext/InputContext.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class HitboxLoader;
class HurtboxLoader;
class PushboxLoader;

class GameScene : public Scene
{
public:
    struct PlayerSlot { std::uint32_t playerId; std::string characterDefPath; };

    struct Config : public Scene::Config
    {
        IPlatformFactory* platformFactory = nullptr;
        std::vector<PlayerSlot> playerSlots;
        std::string mapPath, inputBindingsPath, combosPath, triggersPath;
        float roundTime = 99.f;
    };

    explicit GameScene(Config&& cfg);

    void init() override;
    void update(float deltaTime) override;
    void render() override;

    void onExit() override;

private:
    struct PendingSpawn { std::uint32_t playerId; CharacterLoader::Info result; };

    std::vector<PendingSpawn> pendingSpawns;
    bool spawnEventsEmitted = false;

    float sweepTimer = 0.f, roundTime = 99.f;
    
    EventBus& eventBus;
    SceneManager& sceneManager;
    Renderer& renderer;
    Window& window;
    DataParser& parser;
    ResourceManager& resourceManager;
    TextureLoader& textureLoader;
    GameSettings& settings;
    Engine& engine;
    IFontFactory& fontFactory;
    ITextureFactory& textureFactory;
    IPlatformFactory& platformFactory;

    std::vector<PlayerSlot> playerSlots;
    std::vector<Combo> combos;
    TriggerContext triggerContext;
    std::string mapPath;

    std::unique_ptr<InputContext> inputContext;
    std::unique_ptr<Camera2D> camera;
    std::unique_ptr<CharacterLoader> characterLoader;
    std::unique_ptr<WorldDrawer> worldDrawer;
    std::unique_ptr<EntityFactory> entityFactory;
    std::unique_ptr<CharacterDefinitionLoader> charDefLoader;
    std::unique_ptr<AnimationLoader> animLoader;
    std::unique_ptr<StateMachineLoader> fsmLoader;
    std::unique_ptr<StateMachineRegistry> stateMachineRegistry;

    std::unique_ptr<HitboxLoader> hitboxLoader;
    std::unique_ptr<HurtboxLoader> hurtboxLoader;
    std::unique_ptr<PushboxLoader> pushboxLoader;

    Entity mapRoot;

    void loadInputContext(const std::string& path);
    void setupInputAdapters();
    void loadMap(const std::string& path);
    void loadCombos(const std::string& path);
    void loadTriggerBindings(const std::string& path);

    void prepareScene();
    void prepareComponents();
    void preparePlayers();
    void preparePlayer(const PlayerSlot& slot);

    void addSystems();
};

#endif // game_scene_h
