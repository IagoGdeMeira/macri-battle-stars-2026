#include "../include/SceneFactory/SceneFactory.h"

#include "../include/Engine/Engine.h"
#include "../include/Scene/Scene.h"

#include "../../game/scenes/TitleScene/TitleScene.h"
#include "../../game/scenes/SelectionScene/SelectionScene.h"
#include "../../game/scenes/GameScene/GameScene.h"

#include <stdexcept>

SceneFactory::SceneFactory(
    Window& window,
    InputContext& inputContext,
    TriggerContext& triggerContext,
    CharacterRoster& characterRoster,
    CharacterLoader& characterLoader,
    Camera2D& camera,
    std::vector<Combo>& globalCombos
) :
    window(window),
    inputContext(inputContext),
    triggerContext(triggerContext),
    characterRoster(characterRoster),
    characterLoader(characterLoader),
    camera(camera),
    globalCombos(globalCombos)
{}

std::unique_ptr<Scene> SceneFactory::createScene(SceneId id, std::any data)
{
    if (this->engine == nullptr) throw std::runtime_error("SceneFactory engine is not bound");

    switch (id)
    {
        case SceneId::Title:
        {
            auto& renderer = (this->engine->getRenderer())
                ? *this->engine->getRenderer()
                : throw std::runtime_error("Renderer not set in Engine");
            TitleScene::Config cfg
            {
                this->engine->events(), this->engine->scenes(), this->characterRoster, this->inputContext,
                renderer
            };
            return std::make_unique<TitleScene>(std::move(cfg));
        }
        case SceneId::Selection:
        {
            auto& renderer = (this->engine->getRenderer())
                ? *this->engine->getRenderer()
                : throw std::runtime_error("Renderer not set in Engine");
            SelectionScene::Config cfg
            {
                this->engine->events(), this->engine->scenes(), this->characterRoster,
                this->inputContext, this->triggerContext, this->globalCombos,
                this->camera, this->window, this->characterLoader, renderer
            };
            return std::make_unique<SelectionScene>(std::move(cfg));
        }
        case SceneId::Game:
        {
            auto& renderer = (this->engine->getRenderer())
                ? *this->engine->getRenderer()
                : throw std::runtime_error("Renderer not set in Engine");
            auto slots = std::any_cast<std::vector<GameScene::PlayerSlot>>(std::move(data));
            GameScene::Config cfg
            {
                this->engine->events(), this->inputContext, std::move(this->triggerContext),
                std::move(this->globalCombos), this->camera, this->window, this->characterLoader,
                std::move(slots), renderer
            };
            return std::make_unique<GameScene>(std::move(cfg));
        }
        default: throw std::runtime_error("Unknown SceneId");
    }
}
