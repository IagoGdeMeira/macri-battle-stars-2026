#include "../include/SceneFactory/SceneFactory.h"

#include "../include/Engine/Engine.h"
#include "../include/Scene/Scene.h"

#include "../../game/include/MapLoader/MapLoader.h"
#include "../../game/include/MapRoster/MapRoster.h"
#include "../../game/scenes/TitleScene/TitleScene.h"
#include "../../game/scenes/SelectionScene/SelectionScene.h"
#include "../../game/scenes/GameScene/GameScene.h"

#include <stdexcept>

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
            MapLoader mapLoader(this->parser);
            const auto& maps = this->mapRoster.getAll();
            if (maps.empty()) throw std::runtime_error("No maps available in roster");

            MapData mapData = mapLoader.load(maps.front().definitionPath);
            GameScene::Config cfg
            {
                .eventBus = this->engine->events(),
                .input = this->inputContext,
                .triggerContext = std::move(this->triggerContext),
                .combos = std::move(this->globalCombos),
                .camera = this->camera,
                .window = this->window,
                .characterLoader = this->characterLoader,
                .playerSlots = std::move(slots),
                .renderer = renderer,
                .mapData = std::move(mapData),
                .resourceManager = this->resourceManager,
                .textureLoader = this->textureLoader,
                .settings = this->engine->settings()
            };
            return std::make_unique<GameScene>(std::move(cfg));
        }
        default: throw std::runtime_error("Unknown SceneId");
    }
}
