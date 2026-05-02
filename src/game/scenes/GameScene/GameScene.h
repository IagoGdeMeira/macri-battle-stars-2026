#ifndef game_scene_h
#define game_scene_h

#include "../../include/Camera2D/Camera2D.h"
#include "../../include/CharacterLoader/CharacterLoader.h"
#include "../../include/Combo/Combo.h"
#include "../../include/EntityFactory/EntityFactory.h"
#include "../../include/MapData/MapData.h"
#include "../../include/TriggerContext/TriggerContext.h"
#include "../../include/RenderSystem/RenderSystem.h"

#include "../../../engine/include/EventBus/EventBus.h"
#include "../../../engine/include/InputContext/InputContext.h"
#include "../../../engine/include/Renderer/Renderer.h"
#include "../../../engine/include/ResourceManager/ResourceManager.h"
#include "../../../engine/include/TextureLoader/TextureLoader.h"
#include "../../../engine/include/Scene/Scene.h"
#include "../../../engine/include/Window/Window.h"

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class RenderSystem;

class GameScene : public Scene
{
public:
    struct PlayerSlot { std::uint32_t playerId; std::string characterDefPath; };

    struct Config
    {
        EventBus& eventBus;
        InputContext& input;
        TriggerContext triggerContext;
        std::vector<Combo> combos;
        Camera2D& camera;
        Window& window;
        CharacterLoader& characterLoader;
        std::vector<PlayerSlot> playerSlots;
        Renderer& renderer;
        MapData mapData;
        ResourceManager& resourceManager;
        TextureLoader& textureLoader;
    };

    explicit GameScene(Config&& config);

    void init() override;
    void render() override;

private:
    void prepareScene();
    void prepareComponents();
    void prepareBackgroundLayers();
    void prepareFloor();
    void prepareWalls();
    void preparePlayers();
    void preparePlayer(const PlayerSlot& slot);

    InputContext& input;
    TriggerContext triggerContext;
    Camera2D& camera;
    Window& window;
    std::vector<Combo> combos;
    CharacterLoader& characterLoader;
    std::vector<PlayerSlot> playerSlots;
    Renderer& renderer;
    MapData mapData;
    ResourceManager& resourceManager;
    TextureLoader& textureLoader;

    std::unique_ptr<RenderSystem> renderSystem;
    std::unique_ptr<EntityFactory> entityFactory;
};

#endif // game_scene_h
