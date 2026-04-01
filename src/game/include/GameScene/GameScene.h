#ifndef game_scene_h
#define game_scene_h

#include "../Combo/Combo.h"

#include "../../engine/include/InputContext/InputContext.h"
#include "../../engine/include/Scene/Scene.h"

class GameScene : public Scene
{
public:
    GameScene(EventBus& bus, InputContext& input, std::vector<Combo> combos);
};

#endif // game_scene_h
