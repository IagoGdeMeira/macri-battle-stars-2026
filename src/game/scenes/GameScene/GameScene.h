#ifndef game_scene_h
#define game_scene_h

#include "../../include/Camera2D/Camera2D.h"
#include "../../include/CharacterLoader/CharacterLoader.h"
#include "../../include/Combo/Combo.h"
#include "../../include/StateMachine/StateMachine.h"
#include "../../include/TriggerContext/TriggerContext.h"

#include "../../../engine/include/InputContext/InputContext.h"
#include "../../../engine/include/EventBus/EventBus.h"
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
        std::uint32_t PlayerId;
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
    };

    GameScene(Config&& config);
    void init() override;

private:
    InputContext& input;
    TriggerContext triggerContext;
    StateMachine machine;
    Camera2D& camera;
    Window& window;
    std::vector<Combo> combos;
    CharacterLoader& characterLoader;
    std::vector<PlayerSlot> playerSlots;
};

#endif // game_scene_h
