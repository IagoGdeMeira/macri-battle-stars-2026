#include "../../../src/game/scenes/GameScene.h"

#include "../../stubs/StubDataNode.h"
#include "../../stubs/StubDataParser.h"
#include "../../stubs/StubEngine.h"
#include "../../stubs/StubFontFactory.h"
#include "../../stubs/StubRenderer.h"
#include "../../stubs/StubResourceManager.h"
#include "../../stubs/StubSceneManager.h"
#include "../../stubs/StubTextureFactory.h"
#include "../../stubs/StubWindow.h"

#include "../../../src/domain/components/AirFrictionComponent.h"
#include "../../../src/domain/components/AnalogInputComponent.h"
#include "../../../src/domain/components/GravityComponent.h"
#include "../../../src/domain/components/GroundedComponent.h"
#include "../../../src/domain/components/InputBufferComponent.h"
#include "../../../src/domain/components/InputComponent.h"
#include "../../../src/domain/components/PlayerComponent.h"
#include "../../../src/domain/components/SpriteComponent.h"
#include "../../../src/domain/components/StateComponent.h"
#include "../../../src/domain/components/TransformComponent.h"
#include "../../../src/domain/components/VelocityComponent.h"
#include "../../../src/domain/include/View/View.h"
#include "../../../src/domain/value_objects/StateId/StateId.h"

#include "../../../src/engine/include/DataUtils/DataUtils.h"
#include "../../../src/engine/include/Engine/Engine.h"
#include "../../../src/engine/include/EventBus/EventBus.h"
#include "../../../src/engine/include/GameSettings/GameSettings.h"
#include "../../../src/engine/include/IFontFactory/IFontFactory.h"
#include "../../../src/engine/include/ITextureFactory/ITextureFactory.h"
#include "../../../src/engine/include/Renderer/Renderer.h"
#include "../../../src/engine/include/ResourceManager/ResourceManager.h"
#include "../../../src/engine/include/SceneManager/SceneManager.h"
#include "../../../src/engine/include/TextureLoader/TextureLoader.h"
#include "../../../src/engine/include/Window/Window.h"

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
        root->setFloat("floor.y", 400.f);
        root->setString("floor.texture", "assets/floor.png");
        root->setFloat("floor.width", 2000.f);
        root->setFloat("floor.height", 50.f);
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
        sizeNode->setFloat("w", 64.f);
        sizeNode->setFloat("h", 96.f);
        root->setObject("spriteSize", std::move(sizeNode));
        root->setString("animations", "fake_animations.json");
        root->setString("stateMachine", "fake_fsm.json");
        root->setString("collisions", "");
        root->setArray("customStates", {});
        return root;
    }

    std::unique_ptr<StubDataNode> makeAnimationNode() const
    {
        auto root = std::make_unique<StubDataNode>();
        auto anim = std::make_unique<StubDataNode>();
        anim->setString("state", "Idle");
        anim->setFloat("frameDuration", 0.1f);
        anim->setBool("loop", true);
        auto frame = std::make_unique<StubDataNode>();
        frame->setInt("x", 0);
        frame->setInt("y", 0);
        frame->setInt("width", 64);
        frame->setInt("height", 96);
        std::vector<std::unique_ptr<DataNode>> frames;
        frames.push_back(std::move(frame));
        anim->setArray("frames", std::move(frames));
        std::vector<std::unique_ptr<DataNode>> animations;
        animations.push_back(std::move(anim));
        root->setArray("animations", std::move(animations));
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
        GroundedComponent
    > view(components);

    int count = 0;
    for (auto [entity, player, transform, i_, ib_, v_, g_] : view)
    {
        ++count;
        REQUIRE(player.id == 0);
        REQUIRE(transform.position.x == 300.f);
        REQUIRE(transform.position.y == Catch::Approx(400.f - 48.f));
        (void)entity; (void)i_; (void)ib_; (void)v_; (void)g_;
    }
    REQUIRE(count == 1);
}
