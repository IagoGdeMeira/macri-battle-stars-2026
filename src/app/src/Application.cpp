#include "../include/Application/Application.h"

#include "../../engine/include/Engine/Engine.h"
#include "../../engine/include/InputBindingLoader/InputBindingLoader.h"
#include "../../engine/include/InputManager/InputManager.h"
#include "../../engine/include/ResourceManager/ResourceManager.h"
#include "../../engine/include/ResourceLoader/SyncLoader.h"

#include "../../game/include/AnimationLoader/AnimationLoader.h"
#include "../../game/include/Camera2D/Camera2D.h"
#include "../../game/include/CharacterDefinitionLoader/CharacterDefinitionLoader.h"
#include "../../game/include/CharacterLoader/CharacterLoader.h"
#include "../../game/include/CharacterRoster/CharacterRoster.h"
#include "../../game/include/CharacterRosterLoader/CharacterRosterLoader.h"
#include "../../game/include/Combo/Combo.h"
#include "../../game/include/ComboLoader/ComboLoader.h"
#include "../../game/include/MapData/MapData.h"
#include "../../game/include/MapLoader/MapLoader.h"
#include "../../game/include/MapRosterLoader/MapRosterLoader.h"
#include "../../game/include/StateMachineLoader/StateMachineLoader.h"
#include "../../game/include/TextureLoader/TextureLoader.h"
#include "../../game/include/TriggerBindingLoader/TriggerBindingLoader.h"

#include "../../game/scenes/GameScene/GameScene.h"

#include "../../platform/include/JsonParser/JsonParser.h"
#include "../../platform/include/SDLGamepadAdapter/SDLGamepadAdapter.h"
#include "../../platform/include/SDLKeyboardAdapter/SDLKeyboardAdapter.h"
#include "../../platform/include/SDLMouseAdapter/SDLMouseAdapter.h"
#include "../../platform/include/SDLPlatformEventProvider/SDLPlatformEventProvider.h"
#include "../../platform/include/SDLPlatformInputFactory/SDLPlatformInputFactory.h"
#include "../../platform/include/SDLRenderer/SDLRenderer.h"
#include "../../platform/include/SDLSystemAdapter/SDLSystemAdapter.h"
#include "../../platform/include/SDLSystemInitializer/SDLSystemInitializer.h"
#include "../../platform/include/SDLWindow/SDLWindow.h"

Application& Application::setWindowTitle(const std::string& title)
{
    this->windowTitle = title;
    return *this;
}

Application& Application::setWindowSize(int width, int height)
{
    this->windowWidth = width;
    this->windowHeight = height;
    return *this;
}

int Application::run()
{
    this->initSystems();
    this->initLoaders();
    this->loadGameData();

    this->setupInitialScene();
    this->setupInput();

    this->engine->run();
    return 0;
}

void Application::initSystems()
{
    this->initializer = std::make_unique<SDLSystemInitializer>();
    this->initializer->initialize();

    auto sdlWindow = std::make_unique<SDLWindow>();
    sdlWindow->create(this->windowWidth, this->windowHeight, this->windowTitle.c_str());
    this->window = std::move(sdlWindow);

    this->renderer = std::make_unique<SDLRenderer>(
        static_cast<SDLWindow*>(this->window.get())->getNativeHandle());

    this->camera = std::make_unique<Camera2D>();

}

void Application::initLoaders()
{
    this->parser = std::make_unique<JsonParser>();
    this->threadPool = std::make_unique<ThreadPool>(1);
    this->resourceManager = std::make_unique<ResourceManager>(*this->threadPool);

    this->charDefLoader = std::make_unique<CharacterDefinitionLoader>(*this->parser);
    this->animLoader = std::make_unique<AnimationLoader>(*this->parser);
    this->fsmLoader = std::make_unique<StateMachineLoader>(*this->parser);
    this->textureLoader = std::make_unique<TextureLoader>(*this->renderer);

    this->characterLoader = std::make_unique<CharacterLoader>(
        *this->charDefLoader,
        *this->animLoader,
        *this->fsmLoader,
        *this->resourceManager,
        *this->textureLoader);
}

void Application::loadGameData()
{
    InputBindingLoader inputLoader(*this->parser);
    this->inputContext = std::make_unique<InputContext>(
        inputLoader.load("assets/inputs/input_bindings.json"));

    TriggerBindingLoader triggerLoader(*this->parser);
    this->triggerContext = std::make_unique<TriggerContext>(
        triggerLoader.load("assets/fsm/trigger_bindings.json"));

    ComboLoader comboLoader(*this->parser);
    this->globalCombos = comboLoader.load("assets/combos/combos.json");

    CharacterRosterLoader rosterLoader(*this->parser);
    this->characterRoster = std::make_unique<CharacterRoster>(
        rosterLoader.load("assets/characters/roster.json"));

    MapRosterLoader mapRosterLoader(*this->parser);
    this->mapRoster = std::make_unique<MapRoster>(
        mapRosterLoader.load("assets/maps/roster.json"));
}

void Application::setupInput()
{
    auto& inputManager = this->engine->input();

    auto provider = std::make_unique<SDLPlatformEventProvider>();
    inputManager.setProvider(std::move(provider));

    this->platformInputFactory = std::make_unique<SDLPlatformInputFactory>(this->engine->events());

    inputManager.addAdapter(this->platformInputFactory->createKeyboardAdapter(*this->inputContext));

    auto gamepadAdapters = this->platformInputFactory->createGamepadAdapters(1);
    for (auto& adapter : gamepadAdapters) inputManager.addAdapter(std::move(adapter));

    inputManager.addAdapter(this->platformInputFactory->createSystemAdapter());
}

void Application::setupInitialScene()
{
    this->sceneFactory = std::make_unique<SceneFactory>(
        *this->window,
        *this->inputContext,
        *this->triggerContext,
        *this->characterRoster,
        *this->characterLoader,
        *this->camera,
        this->globalCombos,
        *this->mapRoster,
        *this->parser,
        *this->resourceManager);

    this->engine = std::make_unique<Engine>(*this->window, *this->sceneFactory);
    this->sceneFactory->engine = this->engine.get();
    this->engine->setRenderer(*this->renderer);

    std::vector<GameScene::PlayerSlot> slots;

    const auto& availableCharacters = this->characterRoster->getAll();
    if (availableCharacters.empty()) throw std::runtime_error("No characters found in roster. Cannot start GameScene");

    const std::string& defaultCharId = availableCharacters.front().id;
    const CharacterEntry* defaultEntry = this->characterRoster->findById(defaultCharId);
    if (!defaultEntry) throw std::runtime_error("Default character ID not found in roster.");

    const auto& maps = this->mapRoster->getAll();
    if (maps.empty()) throw std::runtime_error("No maps found in roster. Cannot start GameScene.");

    MapLoader mapLoader(*this->parser);
    MapData defaultMap = mapLoader.load(maps.front().definitionPath);

    for (const auto& [playerId, _] : this->inputContext->bindings)
    { slots.push_back({ .playerId = playerId, .characterDefPath = defaultEntry->definitionPath }); }

    this->engine->scenes().changeScene(
        SceneId::Game,
        std::move(GameScene::Config
        {
            .eventBus = this->engine->events(),
            .input = *this->inputContext,
            .triggerContext = std::move(*this->triggerContext),
            .combos = std::move(this->globalCombos),
            .camera = *this->camera,
            .window = *this->window,
            .characterLoader = *this->characterLoader,
            .playerSlots = std::move(slots),
            .renderer = *this->renderer,
            .mapData = std::move(defaultMap),
            .resourceManager = *this->resourceManager
        }));

}
