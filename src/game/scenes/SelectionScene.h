#ifndef selection_scene_h
#define selection_scene_h

#include "../include/CharacterLoader/CharacterLoader.h"
#include "../include/CharacterRoster/CharacterRoster.h"
#include "../include/Camera2D/Camera2D.h"
#include "../include/Combo/Combo.h"
#include "../include/TriggerContext/TriggerContext.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/InputContext/InputContext.h"
#include "../../engine/include/Renderer/Renderer.h"
#include "../../engine/include/Scene/Scene.h"
#include "../../engine/include/SceneManager/SceneManager.h"
#include "../../engine/include/Window/Window.h"

#include <cstdint>
#include <unordered_map>

class SelectionScene : public Scene 
{
public:
    struct Config : public Scene::Config {};

    explicit SelectionScene(Config&& cfg);

    void update(float deltaTime) override { (void)deltaTime; }

private:
    std::unordered_map<uint32_t, std::string> selections;
    bool confirmed = false;
};

#endif // selection_scene_h
