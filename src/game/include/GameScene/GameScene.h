#ifndef game_scene_h
#define game_scene_h

#include "../Camera2D/Camera2D.h"
#include "../Combo/Combo.h"
#include "../StateMachine/StateMachine.h"

#include "../../engine/include/InputContext/InputContext.h"
#include "../../engine/include/Scene/Scene.h"
#include "../../engine/include/Window/Window.h"


class GameScene : public Scene
{
public:
    GameScene(
        EventBus& bus,
        InputContext& input,
        std::vector<Combo> combos,
        StateMachine machine,
        Camera2D& camera,
        Window& window
    );

    void init() override;

private:
    InputContext& input;
    StateMachine machine;

    Camera2D& camera;
    Window& window;
};

#endif // game_scene_h
