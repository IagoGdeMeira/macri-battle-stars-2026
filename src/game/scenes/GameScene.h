#ifndef game_scene_h
#define game_scene_h

#include "../include/Camera2D/Camera2D.h"
#include "../include/CharacterLoader/CharacterLoader.h"
#include "../include/Combo/Combo.h"
#include "../include/EntityFactory/EntityFactory.h"
#include "../include/TriggerContext/TriggerContext.h"
#include "../include/WorldDrawer/WorldDrawer.h"

#include "../../domain/components/MapComponent.h"
#include "../../domain/include/Entity/Entity.h"

#include "../../engine/include/DataParser/DataParser.h"
#include "../../engine/include/Engine/Engine.h"
#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/IFontFactory/IFontFactory.h"
#include "../../engine/include/InputContext/InputContext.h"
#include "../../engine/include/IPlatformFactory/IPlatformFactory.h"
#include "../../engine/include/ITextureFactory/ITextureFactory.h"
#include "../../engine/include/Renderer/Renderer.h"
#include "../../engine/include/ResourceManager/ResourceManager.h"
#include "../../engine/include/Scene/Scene.h"
#include "../../engine/include/SceneManager/SceneManager.h"
#include "../../engine/include/TextureLoader/TextureLoader.h"
#include "../../engine/include/Window/Window.h"
#include "../../engine/value_objects/GameSettings/GameSettings.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class GameScene : public Scene
{
public:
    struct PlayerSlot { std::uint32_t playerId; std::string characterDefPath; };

    struct Config : public Scene::Config
    {
        IPlatformFactory* platformFactory = nullptr;
        std::vector<PlayerSlot> playerSlots;
        std::string mapPath, inputBindingsPath, combosPath, triggersPath;
    };

    explicit GameScene(Config&& cfg);

    void init() override;
    void render() override;

    Scene::UpdatePolicy getUpdatePolicy() const override { return Scene::UpdatePolicy::WhenTop; }

private:
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
    std::unique_ptr<CollisionClipLoader> clipLoader;

    Entity mapRoot;

    void loadInputContext(const std::string& path);
    void setupInputAdapters();
    void loadMap(const std::string& path);
    void loadCombos(const std::string& path);
    void loadTriggerBindings(const std::string& path);
    void createCharacterLoader();

    void prepareScene();
    void prepareComponents();
    void preparePlayers();
    void preparePlayer(const PlayerSlot& slot);
};

#endif // game_scene_h
