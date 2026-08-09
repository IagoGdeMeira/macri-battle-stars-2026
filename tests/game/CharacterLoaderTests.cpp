#include "game/include/CharacterLoader/CharacterLoader.h"

#include "StubAnimationLoader.h"
#include "StubDataNode.h"
#include "StubDataParser.h"
#include "StubHitboxLoader.h"
#include "StubHurtboxLoader.h"
#include "StubPushboxLoader.h"
#include "StubRenderer.h"
#include "StubResourceManager.h"
#include "StubTextureFactory.h"
#include "StubTextureLoader.h"

#include "domain/components/AnimationComponent.h"
#include "domain/components/AnimationControllerComponent.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/HitboxControllerComponent.h"
#include "domain/components/HitboxControllerMapComponent.h"
#include "domain/components/HurtboxControllerComponent.h"
#include "domain/components/HurtboxControllerMapComponent.h"
#include "domain/components/JumpComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/components/PushboxControllerMapComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/StateComponent.h"
#include "domain/components/StateMachineComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/StateId/StateId.h"

#include "engine/include/DataParser/DataParser.h"
#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/TextureLoader/TextureLoader.h"

#include "game/include/AnimationLoader/AnimationLoader.h"
#include "game/include/CharacterDefinitionLoader/CharacterDefinitionLoader.h"
#include "game/include/EntityFactory/EntityFactory.h"
#include "game/include/HitboxLoader/HitboxLoader.h"
#include "game/include/HurtboxLoader/HurtboxLoader.h"
#include "game/include/PushboxLoader/PushboxLoader.h"
#include "game/include/StateIdMapper/StateIdMapper.h"
#include "game/include/StateMachineLoader/StateMachineLoader.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <vector>

class CharacterLoaderFixture
{
public:
    std::unique_ptr<StubDataNode> makeSpriteSize(float w, float h) const
    {
        auto sizeNode = std::make_unique<StubDataNode>();
        sizeNode->setFloat("width", w);
        sizeNode->setFloat("height", h);
        return sizeNode;
    }

    struct JumpParams { float force = 1500.f, maxTime = 0.2f, gravityScaleAsc = 0.6f, gravityScaleDesc = 1.8f, fastFallMultiplier = 2.5f; };
    std::unique_ptr<StubDataNode> makeJumpNode(const JumpParams& params = JumpParams{}) const
    {
        auto jumpNode = std::make_unique<StubDataNode>();
        jumpNode->setFloat("force", params.force);
        jumpNode->setFloat("maxTime", params.maxTime);
        jumpNode->setFloat("gravityScaleAsc", params.gravityScaleAsc);
        jumpNode->setFloat("gravityScaleDesc", params.gravityScaleDesc);
        jumpNode->setFloat("fastFallMultiplier", params.fastFallMultiplier);
        return jumpNode;
    }

    std::unique_ptr<StubDataNode> makeEmptyCollisionsRoot() const
    {
        auto root = std::make_unique<StubDataNode>();
        root->setArray("states", std::vector<std::unique_ptr<DataNode>>());
        return root;
    }

    std::unique_ptr<StubDataNode> makeDefinitionRoot() const
    {
        auto root = std::make_unique<StubDataNode>();
        root->setString("id", "fighter_01");
        root->setString("texture", "assets/sprites/fighter.png");
        root->setObject("spriteSize", this->makeSpriteSize(64.f, 96.f));
        root->setString("animations", "assets/animations/fighter_01.json");
        root->setString("stateMachine", "assets/fsm/fighter_01.json");
        root->setString("collisions", "assets/collisions/fighter_01.json");
        root->setObject("jump", this->makeJumpNode({1800.f, 0.25f, 0.7f, 1.9f, 2.2f}));
        return root;
    }

    std::unique_ptr<StubDataNode> makeStateMachineRoot() const
    {
        auto trans = std::make_unique<StubDataNode>();
        trans->setString("from", "Idle");
        trans->setString("to", "Punching");
        trans->setString("trigger", "Punched");

        auto root = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> transitions;
        transitions.push_back(std::move(trans));
        root->setArray("transitions", std::move(transitions));
        return root;
    }

    std::unique_ptr<StubDataNode> makeCollisionsRoot() const { return this->makeEmptyCollisionsRoot(); }
};

TEST_CASE_METHOD(CharacterLoaderFixture, "CharacterLoader creates entity and required gameplay components",
    "[integration][character_loader]"
) {
    StubDataParser defParser, animParser, fsmParser, collParser;
    defParser.registerNode("assets/characters/fighter_01.json", this->makeDefinitionRoot());
    fsmParser.registerNode("assets/fsm/fighter_01.json", this->makeStateMachineRoot());
    collParser.registerNode("assets/collisions/fighter_01.json", this->makeCollisionsRoot());

    auto idleFrame = std::make_unique<StubDataNode>();
    idleFrame->setInt("x", 0);
    idleFrame->setInt("y", 0);
    idleFrame->setInt("width", 64);
    idleFrame->setInt("height", 96);

    auto idleAnimNode = std::make_unique<StubDataNode>();
    idleAnimNode->setFloat("frameDuration", 0.12f);
    idleAnimNode->setBool("loop", true);
    std::vector<std::unique_ptr<DataNode>> idleFrames;
    idleFrames.push_back(std::move(idleFrame));
    idleAnimNode->setArray("frames", std::move(idleFrames));
    animParser.registerNode("idle_anim.json", std::move(idleAnimNode));

    auto animEntry = std::make_unique<StubDataNode>();
    animEntry->setString("name", "Idle");
    animEntry->setString("path", "idle_anim.json");

    auto animIndexRoot = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> animStates;
    animStates.push_back(std::move(animEntry));
    animIndexRoot->setArray("states", std::move(animStates));
    animParser.registerNode("assets/animations/fighter_01.json", std::move(animIndexRoot));

    CharacterDefinitionLoader defLoader(defParser);
    AnimationLoader animLoader(animParser);
    StateMachineLoader fsmLoader(fsmParser);

    StubResourceManager resourceManager;
    StubTextureFactory textureFactory;
    StubTextureLoader textureLoader(textureFactory);
    StubAnimationLoader animLoaderStub;
    StateMachineRegistry registry;

    World world;
    
    auto entityFactory = std::make_unique<EntityFactory>(EntityFactory::Config{ 
        world, resourceManager, textureLoader, animLoaderStub});

    HitboxLoader hitboxLoader(collParser, *entityFactory);
    HurtboxLoader hurtboxLoader(collParser, *entityFactory);
    PushboxLoader pushboxLoader(collParser, *entityFactory);

    CharacterLoader loader(CharacterLoader::Config{
        .parser                 = collParser,
        .defLoader              = defLoader,
        .animLoader             = animLoader,
        .fsmLoader              = fsmLoader,
        .resourceManager        = resourceManager,
        .textureLoader          = textureLoader,
        .hitboxLoader           = hitboxLoader,
        .hurtboxLoader          = hurtboxLoader,
        .pushboxLoader          = pushboxLoader,
        .stateMachineRegistry   = registry
    });

    auto& comp = world.components();
    comp.registerComponent<AnimationComponent>();
    comp.registerComponent<AnimationControllerComponent>();
    comp.registerComponent<ChildrenComponent>();
    comp.registerComponent<HitboxControllerComponent>();
    comp.registerComponent<HitboxControllerMapComponent>();
    comp.registerComponent<HurtboxControllerComponent>();
    comp.registerComponent<HurtboxControllerMapComponent>();
    comp.registerComponent<JumpComponent>();
    comp.registerComponent<LocalTransform>();
    comp.registerComponent<ParentComponent>();
    comp.registerComponent<PushboxControllerComponent>();
    comp.registerComponent<PushboxControllerMapComponent>();
    comp.registerComponent<RenderComponent>();
    comp.registerComponent<SpriteComponent>();
    comp.registerComponent<StateComponent>();
    comp.registerComponent<StateMachineComponent>();
    comp.registerComponent<TransformComponent>();

    const auto entity = loader.create(world, "assets/characters/fighter_01.json");

    REQUIRE(comp.has<StateComponent>(entity));
    REQUIRE(comp.has<StateMachineComponent>(entity));
    REQUIRE(comp.has<AnimationControllerComponent>(entity));
    REQUIRE(comp.has<JumpComponent>(entity));

    const auto& jumpComp = comp.get<JumpComponent>(entity);
    REQUIRE(jumpComp.force == Catch::Approx(1800.f));
    REQUIRE(jumpComp.maxTime == Catch::Approx(0.25f));
    REQUIRE(jumpComp.gravityScaleAsc == Catch::Approx(0.7f));
    REQUIRE(jumpComp.gravityScaleDesc == Catch::Approx(1.9f));
    REQUIRE(jumpComp.fastFallMultiplier == Catch::Approx(2.2f));

    const auto& state = comp.get<StateComponent>(entity);
    REQUIRE(state.current == StateId::Idle);

    uint32_t machineId = comp.get<StateMachineComponent>(entity).machineId;
    const StateMachine* machine = registry.getMachine(machineId);
    REQUIRE(machine != nullptr);
    REQUIRE(machine->transitions.size() == 1);
    REQUIRE(machine->transitions[0].from == StateId::Idle);
    REQUIRE(machine->transitions[0].to == StateId::Punching);

    const auto& controller = comp.get<AnimationControllerComponent>(entity);
    REQUIRE(controller.animations.right.size() == 1);
    REQUIRE(controller.animations.right.contains(StateId::Idle));
    REQUIRE(controller.currentState == StateId::Idle);

    REQUIRE(comp.has<HitboxControllerMapComponent>(entity));
    REQUIRE(comp.has<HurtboxControllerMapComponent>(entity));
    REQUIRE(comp.has<PushboxControllerMapComponent>(entity));

    REQUIRE(comp.has<ChildrenComponent>(entity));
    const auto& children = comp.get<ChildrenComponent>(entity);
    REQUIRE(children.children.size() == 1);
    Entity visualEntity = children.children[0];

    REQUIRE(comp.has<SpriteComponent>(visualEntity));
    REQUIRE(comp.has<AnimationComponent>(visualEntity));

    const auto& sprite = comp.get<SpriteComponent>(visualEntity);
    REQUIRE(sprite.texturePath == "assets/sprites/fighter.png");
    REQUIRE(sprite.source.size.width == 64);
    REQUIRE(sprite.source.size.height == 96);
    REQUIRE(sprite.useSourceRect == false);

    const auto& animation = comp.get<AnimationComponent>(visualEntity);
    REQUIRE(animation.currentFrame == 0);
    REQUIRE(animation.elapsedTime == 0.f);
    REQUIRE(animation.currentState == StateId::Idle);
}

TEST_CASE_METHOD(CharacterLoaderFixture, "CharacterLoader resolves custom states per character definition",
    "[integration][character_loader]"
) {
    StubDataParser animParser, collParser, fsmParser, defParser;

    auto defRoot = std::make_unique<StubDataNode>();
    defRoot->setString("id", "fighter_custom");
    defRoot->setString("texture", "assets/sprites/fighter.png");
    defRoot->setObject("spriteSize", this->makeSpriteSize(64.f, 96.f));
    defRoot->setString("animations", "assets/animations/fighter_custom.json");
    defRoot->setString("stateMachine", "assets/fsm/fighter_custom.json");
    defRoot->setString("collisions", "assets/collisions/fighter_custom.json");
    defRoot->setObject("jump", this->makeJumpNode());

    auto customStateNode = std::make_unique<StubDataNode>();
    customStateNode->setString("", "PowerCharge");
    std::vector<std::unique_ptr<DataNode>> customStates;
    customStates.push_back(std::move(customStateNode));
    defRoot->setArray("customStates", std::move(customStates));

    defParser.registerNode("def.json", std::move(defRoot));

    auto idleFrame = std::make_unique<StubDataNode>();
    idleFrame->setInt("x", 0); idleFrame->setInt("y", 0);
    idleFrame->setInt("width", 64); idleFrame->setInt("height", 96);

    auto idleAnimNode = std::make_unique<StubDataNode>();
    idleAnimNode->setFloat("frameDuration", 0.1f);
    idleAnimNode->setBool("loop", true);
    std::vector<std::unique_ptr<DataNode>> idleFrames;
    idleFrames.push_back(std::move(idleFrame));
    idleAnimNode->setArray("frames", std::move(idleFrames));
    animParser.registerNode("idle_anim.json", std::move(idleAnimNode));

    auto pcFrame = std::make_unique<StubDataNode>();
    pcFrame->setInt("x", 0); pcFrame->setInt("y", 0);
    pcFrame->setInt("width", 64); pcFrame->setInt("height", 96);

    auto pcAnimNode = std::make_unique<StubDataNode>();
    pcAnimNode->setFloat("frameDuration", 0.2f);
    pcAnimNode->setBool("loop", false);
    std::vector<std::unique_ptr<DataNode>> pcFrames;
    pcFrames.push_back(std::move(pcFrame));
    pcAnimNode->setArray("frames", std::move(pcFrames));
    animParser.registerNode("powercharge_anim.json", std::move(pcAnimNode));

    auto idxRoot = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> states;
    auto entryIdle = std::make_unique<StubDataNode>();
    entryIdle->setString("name", "Idle");
    entryIdle->setString("path", "idle_anim.json");
    states.push_back(std::move(entryIdle));

    auto entryPC = std::make_unique<StubDataNode>();
    entryPC->setString("name", "PowerCharge");
    entryPC->setString("path", "powercharge_anim.json");
    states.push_back(std::move(entryPC));

    idxRoot->setArray("states", std::move(states));
    animParser.registerNode("assets/animations/fighter_custom.json", std::move(idxRoot));

    auto trans = std::make_unique<StubDataNode>();
    trans->setString("from", "Idle");
    trans->setString("to", "PowerCharge");
    trans->setString("trigger", "Punched");
    auto fsmRoot = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> transitions;
    transitions.push_back(std::move(trans));
    fsmRoot->setArray("transitions", std::move(transitions));
    fsmParser.registerNode("assets/fsm/fighter_custom.json", std::move(fsmRoot));

    auto collRoot = this->makeEmptyCollisionsRoot();
    collParser.registerNode("assets/collisions/fighter_custom.json", std::move(collRoot));

    CharacterDefinitionLoader defLoader(defParser);
    AnimationLoader animLoader(animParser);
    StateMachineLoader fsmLoader(fsmParser);

    StubResourceManager resourceManager;
    StubTextureFactory textureFactory;
    StubTextureLoader textureLoader(textureFactory);
    StubAnimationLoader animLoaderStub;
    StateMachineRegistry registry;

    World world;
    auto entityFactory = std::make_unique<EntityFactory>(EntityFactory::Config{
        world, resourceManager, textureLoader, animLoaderStub});

    HitboxLoader hitboxLoader(collParser, *entityFactory);
    HurtboxLoader hurtboxLoader(collParser, *entityFactory);
    PushboxLoader pushboxLoader(collParser, *entityFactory);

    CharacterLoader loader(CharacterLoader::Config{
        .parser                 = collParser,
        .defLoader              = defLoader,
        .animLoader             = animLoader,
        .fsmLoader              = fsmLoader,
        .resourceManager        = resourceManager,
        .textureLoader          = textureLoader,
        .hitboxLoader           = hitboxLoader,
        .hurtboxLoader          = hurtboxLoader,
        .pushboxLoader          = pushboxLoader,
        .stateMachineRegistry   = registry
    });

    auto& comp = world.components();
    comp.registerComponent<AnimationComponent>();
    comp.registerComponent<AnimationControllerComponent>();
    comp.registerComponent<ChildrenComponent>();
    comp.registerComponent<HitboxControllerComponent>();
    comp.registerComponent<HitboxControllerMapComponent>();
    comp.registerComponent<HurtboxControllerComponent>();
    comp.registerComponent<HurtboxControllerMapComponent>();
    comp.registerComponent<JumpComponent>();
    comp.registerComponent<LocalTransform>();
    comp.registerComponent<ParentComponent>();
    comp.registerComponent<PushboxControllerComponent>();
    comp.registerComponent<PushboxControllerMapComponent>();
    comp.registerComponent<RenderComponent>();
    comp.registerComponent<SpriteComponent>();
    comp.registerComponent<StateComponent>();
    comp.registerComponent<StateMachineComponent>();
    comp.registerComponent<TransformComponent>();

    const auto entity = loader.create(world, "def.json");

    REQUIRE(comp.has<JumpComponent>(entity));

    uint32_t machineId = comp.get<StateMachineComponent>(entity).machineId;
    const StateMachine* machine = registry.getMachine(machineId);
    REQUIRE(machine != nullptr);
    REQUIRE(machine->transitions.size() == 1);

    StateId customStateId = machine->transitions[0].to;
    REQUIRE(customStateId.isCustom());

    const auto& controller = comp.get<AnimationControllerComponent>(entity);
    REQUIRE(controller.animations.right.contains(customStateId));
    REQUIRE(comp.has<HitboxControllerMapComponent>(entity));
    REQUIRE(comp.has<HurtboxControllerMapComponent>(entity));
    REQUIRE(comp.has<PushboxControllerMapComponent>(entity));
}
