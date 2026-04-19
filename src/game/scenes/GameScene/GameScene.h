#ifndef game_scene_h
#define game_scene_h

#include "../../include/Camera2D/Camera2D.h"
#include "../../include/CharacterLoader/CharacterLoader.h"
#include "../../include/Combo/Combo.h"
#include "../../include/RenderSystem/RenderSystem.h"
#include "../../include/StateMachine/StateMachine.h"
#include "../../include/TriggerContext/TriggerContext.h"

#include "../../../engine/include/InputContext/InputContext.h"
#include "../../../engine/include/EventBus/EventBus.h"
#include "../../../engine/include/Renderer/Renderer.h"
#include "../../../engine/include/Scene/Scene.h"
#include "../../../engine/include/Window/Window.h"

#include <cstdint>
#include <string>
#include <vector>

class GameScene : public Scene
{
public:
    struct PlayerSlot
    {
        std::uint32_t playerId;
        std::string characterDefPath;
    };

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
    };

    GameScene(Config&& config);

    void init() override;
    void render() override;

private:
    InputContext& input;
    TriggerContext triggerContext;
    Camera2D& camera;
    Window& window;
    std::vector<Combo> combos;
    CharacterLoader& characterLoader;
    std::vector<PlayerSlot> playerSlots;
    Renderer& renderer;

    std::unique_ptr<RenderSystem> renderSystem;
};

#endif // game_scene_h
