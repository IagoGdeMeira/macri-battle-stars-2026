#include "game/scenes/GameScene.h"

#include "StubDataNode.h"
#include "StubDataParser.h"
#include "StubEngine.h"
#include "StubFontFactory.h"
#include "StubPlatformFactory.h"
#include "StubRenderer.h"
#include "StubResourceManager.h"
#include "StubSceneManager.h"
#include "StubTextureFactory.h"
#include "StubWindow.h"

#include "domain/components/AirFrictionComponent.h"
#include "domain/components/AnalogInputComponent.h"
#include "domain/components/GravityComponent.h"
#include "domain/components/GroundedComponent.h"
#include "domain/components/InputBufferComponent.h"
#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/StateComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/View/View.h"
#include "domain/value_objects/StateId/StateId.h"

#include "engine/include/Engine/Engine.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/include/IFontFactory/IFontFactory.h"
#include "engine/include/ITextureFactory/ITextureFactory.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/SceneManager/SceneManager.h"
#include "engine/include/TextureLoader/TextureLoader.h"
#include "engine/include/Window/Window.h"
#include "engine/utils/DataUtils/DataUtils.h"
#include "engine/value_objects/GameSettings/GameSettings.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <vector>

class GameSceneFixture
{
public:
    StubWindow window;
    StubRenderer renderer;
    GameSettings settings;
    StubResourceManager resourceManager;
    StubTextureFactory textureFactory;
    StubFontFactory fontFactory;
    StubDataParser parser;
    TextureLoader textureLoader{this->textureFactory};
    StubEngine engine{this->window, this->settings};
    StubSceneManager sceneManager;
    StubPlatformFactory platformFactory;

    GameSceneFixture()
    {
        this->parser.registerNode("assets/maps/stage1.json", this->makeMapNode());
        this->parser.registerNode("assets/inputs/game_bindings.json", this->makeInputBindingsNode());
        this->parser.registerNode("assets/combos/default_combos.json", this->makeCombosNode());
        this->parser.registerNode("assets/triggers/default_triggers.json", this->makeTriggersNode());
        this->parser.registerNode("assets/characters/knight.json", this->makeCharacterDefNode());
        this->parser.registerNode("fake_animations.json", this->makeAnimationNode());
        this->parser.registerNode("fake_fsm.json", this->makeFSMNode());
    }

    std::unique_ptr<StubDataNode> makeMapNode() const
    {
        auto root = std::make_unique<StubDataNode>();
        root->setString("name", "TestStage");
        auto floorNode = std::make_unique<StubDataNode>();
        floorNode->setFloat("y", 400.f);
        floorNode->setString("texture", "assets/floor.png");
        floorNode->setFloat("width", 2000.f);
        floorNode->setFloat("height", 50.f);
        root->setObject("floor", std::move(floorNode));
        auto spawn = std::make_unique<StubDataNode>();
        spawn->setInt("playerId", 0);
        spawn->setFloat("x", 300.f);
        std::vector<std::unique_ptr<DataNode>> spawns;
        spawns.push_back(std::move(spawn));
        root->setArray("spawnPoints", std::move(spawns));
        root->setArray("walls", {});
        root->setArray("backgroundLayers", {});
        root->setFloat("gravity", 980.f);
        root->setFloat("floorFriction", 5.f);
        root->setFloat("airFriction", 2.f);
        return root;
    }

    std::unique_ptr<StubDataNode> makeInputBindingsNode() const
    {
        auto root = std::make_unique<StubDataNode>();
        auto player = std::make_unique<StubDataNode>();
        player->setInt("id", 0);
        player->setArray("bindings", {});
        std::vector<std::unique_ptr<DataNode>> players;
        players.push_back(std::move(player));
        root->setArray("players", std::move(players));
        return root;
    }

    std::unique_ptr<StubDataNode> makeCombosNode() const
    {
        auto root = std::make_unique<StubDataNode>();
        root->setArray("combos", {});
        return root;
    }

    std::unique_ptr<StubDataNode> makeTriggersNode() const
    {
        auto root = std::make_unique<StubDataNode>();
        root->setArray("bindings", {});
        return root;
    }

    std::unique_ptr<StubDataNode> makeCharacterDefNode() const
    {
        auto root = std::make_unique<StubDataNode>();
        root->setString("id", "test_fighter");
        root->setString("texture", "assets/sprites/fighter.png");

        auto sizeNode = std::make_unique<StubDataNode>();
        sizeNode->setFloat("width", 64.f);
        sizeNode->setFloat("height", 96.f);
        root->setObject("spriteSize", std::move(sizeNode));
        root->setString("animations", "fake_animations.json");
        root->setString("stateMachine", "fake_fsm.json");
        root->setString("collisions", "");
        root->setArray("customStates", {});
        return root;
    }

    std::unique_ptr<StubDataNode> makeAnimationNode()
    {
        auto frame = std::make_unique<StubDataNode>();
        frame->setInt("x", 0);
        frame->setInt("y", 0);
        frame->setInt("width", 64);
        frame->setInt("height", 96);

        auto idleAnim = std::make_unique<StubDataNode>();
        idleAnim->setFloat("frameDuration", 0.1f);
        idleAnim->setBool("loop", true);
        std::vector<std::unique_ptr<DataNode>> frames;
        frames.push_back(std::move(frame));
        idleAnim->setArray("frames", std::move(frames));

        this->parser.registerNode("idle_anim.json", std::move(idleAnim));

        auto entry = std::make_unique<StubDataNode>();
        entry->setString("name", "Idle");
        entry->setString("path", "idle_anim.json");

        auto root = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> states;
        states.push_back(std::move(entry));
        root->setArray("states", std::move(states));

        return root;
    }
    
    std::unique_ptr<StubDataNode> makeFSMNode() const
    {
        auto root = std::make_unique<StubDataNode>();
        auto idleState = std::make_unique<StubDataNode>();
        idleState->setString("name", "Idle");
        std::vector<std::unique_ptr<DataNode>> states;
        states.push_back(std::move(idleState));
        root->setArray("states", std::move(states));

        auto transition = std::make_unique<StubDataNode>();
        transition->setString("from", "Idle");
        transition->setString("to", "Idle");
        transition->setString("trigger", "Jumped");
        transition->setArray("conditions", {});

        std::vector<std::unique_ptr<DataNode>> transitions;
        transitions.push_back(std::move(transition));
        root->setArray("transitions", std::move(transitions));
        return root;
    }
};

TEST_CASE_METHOD(GameSceneFixture, "GameScene initializes and creates player entity",
    "[integration][game_scene]"
) {
    GameScene::Config cfg;
    cfg.eventBus        = &this->engine.events();
    cfg.sceneManager    = &this->sceneManager;
    cfg.renderer        = &this->renderer;
    cfg.window          = &this->window;
    cfg.parser          = &this->parser;
    cfg.resourceManager = &this->resourceManager;
    cfg.textureLoader   = &this->textureLoader;
    cfg.settings        = &this->settings;
    cfg.engine          = &this->engine;
    cfg.fontFactory     = &this->fontFactory;
    cfg.textureFactory  = &this->textureFactory;
    cfg.platformFactory = nullptr;

    cfg.playerSlots         = { {0, "assets/characters/knight.json"} };
    cfg.mapPath             = "assets/maps/stage1.json";
    cfg.inputBindingsPath   = "assets/inputs/game_bindings.json";
    cfg.combosPath          = "assets/combos/default_combos.json";
    cfg.triggersPath        = "assets/triggers/default_triggers.json";

    GameScene scene(std::move(cfg));
    scene.init();

    auto& world = scene.world();
    auto& components = world.components();

    View<
        PlayerComponent,
        TransformComponent,
        InputComponent,
        InputBufferComponent,
        VelocityComponent,
        GroundedComponent,
        RenderComponent
    > view(components);

    int count = 0;
    for (auto [entity, player, transform, i_, ib_, v_, g_, r] : view)
    {
        ++count;
        REQUIRE(player.id == 0);
        REQUIRE(transform.position.x == 300.f);
        REQUIRE(transform.position.y == Catch::Approx(400.f - 48.f));
        (void)entity, i_, ib_, v_, g_, r;
    }
    REQUIRE(count == 1);
}
