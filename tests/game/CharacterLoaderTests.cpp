#include "../../src/game/include/CharacterLoader/CharacterLoader.h"

#include "../stubs/StubDataNode.h"
#include "../stubs/StubDataParser.h"
#include "../stubs/StubRenderer.h"
#include "../stubs/StubResourceManager.h"
#include "../stubs/StubTextureFactory.h"

#include "../../src/domain/components/AnimationComponent.h"
#include "../../src/domain/components/AnimationControllerComponent.h"
#include "../../src/domain/components/CollisionClipDefinitionsComponent.h"
#include "../../src/domain/components/CollisionClipPlayerComponent.h"
#include "../../src/domain/components/SpriteComponent.h"
#include "../../src/domain/components/StateComponent.h"
#include "../../src/domain/components/StateMachineComponent.h"
#include "../../src/domain/components/StateMappingComponent.h"
#include "../../src/domain/include/StateId/StateId.h"
#include "../../src/domain/include/TriggerId/TriggerId.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/DataParser/DataParser.h"
#include "../../src/engine/include/ResourceManager/ResourceManager.h"
#include "../../src/engine/include/TextureLoader/TextureLoader.h"

#include "../../src/game/include/AnimationLoader/AnimationLoader.h"
#include "../../src/game/include/CharacterDefinitionLoader/CharacterDefinitionLoader.h"
#include "../../src/game/include/CollisionClipLoader/CollisionClipLoader.h"
#include "../../src/game/include/StateIdMapper/StateIdMapper.h"
#include "../../src/game/include/StateMachineLoader/StateMachineLoader.h"

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
        sizeNode->setFloat("w", w);
        sizeNode->setFloat("h", h);
        return sizeNode;
    }

    std::unique_ptr<StubDataNode> makeEmptyClipsRoot() const
    {
        auto root = std::make_unique<StubDataNode>();
        root->setArray("clips", std::vector<std::unique_ptr<DataNode>>());
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
        return root;
    }

    std::unique_ptr<StubDataNode> makeAnimationRoot() const
    {
        auto frame = std::make_unique<StubDataNode>();
        frame->setInt("x", 0);
        frame->setInt("y", 0);
        frame->setInt("width", 64);
        frame->setInt("height", 96);

        auto anim = std::make_unique<StubDataNode>();
        anim->setString("state", "Idle");
        anim->setFloat("frameDuration", 0.12f);
        anim->setBool("loop", true);
        std::vector<std::unique_ptr<DataNode>> frames;
        frames.push_back(std::move(frame));
        anim->setArray("frames", std::move(frames));

        auto root = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> animations;
        animations.push_back(std::move(anim));
        root->setArray("animations", std::move(animations));
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
};

TEST_CASE_METHOD(CharacterLoaderFixture, "CharacterLoader creates entity and required gameplay components",
    "[integration][character_loader]"
) {
    StubDataParser defParser, animParser, fsmParser, clipParser;
    defParser.registerNode("assets/characters/fighter_01.json", this->makeDefinitionRoot());
    animParser.registerNode("assets/animations/fighter_01.json", this->makeAnimationRoot());
    fsmParser.registerNode("assets/fsm/fighter_01.json", this->makeStateMachineRoot());
    clipParser.registerNode("assets/collisions/fighter_01.json", this->makeEmptyClipsRoot());

    CharacterDefinitionLoader defLoader(defParser);
    AnimationLoader animLoader(animParser);
    StateMachineLoader fsmLoader(fsmParser);
    CollisionClipLoader clipLoader(clipParser);

    StubResourceManager resourceManager;
    StubTextureFactory textureFactory;
    TextureLoader textureLoader(textureFactory);

    CharacterLoader loader(CharacterLoader::Config{
        .defLoader          = defLoader,
        .animLoader         = animLoader,
        .fsmLoader          = fsmLoader,
        .resourceManager    = resourceManager,
        .textureLoader      = textureLoader,
        .clipLoader         = clipLoader
    });

    World world;
    auto& components = world.components();
    components.registerComponent<SpriteComponent>();
    components.registerComponent<StateComponent>();
    components.registerComponent<StateMappingComponent>();
    components.registerComponent<StateMachineComponent>();
    components.registerComponent<AnimationControllerComponent>();
    components.registerComponent<AnimationComponent>();
    components.registerComponent<CollisionClipDefinitionsComponent>();
    components.registerComponent<CollisionClipPlayerComponent>();

    const auto entity = loader.create(world, "assets/characters/fighter_01.json");

    REQUIRE(components.has<SpriteComponent>(entity));
    REQUIRE(components.has<StateComponent>(entity));
    REQUIRE(components.has<StateMappingComponent>(entity));
    REQUIRE(components.has<StateMachineComponent>(entity));
    REQUIRE(components.has<AnimationControllerComponent>(entity));
    REQUIRE(components.has<AnimationComponent>(entity));

    const auto& sprite = components.get<SpriteComponent>(entity);
    REQUIRE(sprite.texture == textureFactory.textureToReturn);
    REQUIRE(sprite.source.size.width == 64);
    REQUIRE(sprite.source.size.height == 96);
    REQUIRE(sprite.useSourceRect == false);

    const auto& state = components.get<StateComponent>(entity);
    REQUIRE(state.current == StateId::Idle);
    REQUIRE(state.timeInState == 0.f);

    const auto& stateMachine = components.get<StateMachineComponent>(entity);
    REQUIRE(stateMachine.machine.transitions.size() == 1);
    REQUIRE(stateMachine.machine.transitions[0].from == StateId::Idle);
    REQUIRE(stateMachine.machine.transitions[0].to == StateId::Punching);

    const auto& controller = components.get<AnimationControllerComponent>(entity);
    REQUIRE(controller.animations.right.size() == 1);
    REQUIRE(controller.animations.right.contains(StateId::Idle));
    REQUIRE(controller.currentState == StateId::Unknown);

    const auto& animation = components.get<AnimationComponent>(entity);
    REQUIRE(animation.currentFrame == 0);
    REQUIRE(animation.elapsedTime == 0.f);
    REQUIRE(animation.currentState == StateId::Unknown);
}

TEST_CASE_METHOD(CharacterLoaderFixture, "CharacterLoader resolves custom states per character definition",
    "[integration][character_loader]"
) {
    auto defRoot = std::make_unique<StubDataNode>();
    defRoot->setString("id", "fighter_custom");
    defRoot->setString("texture", "assets/sprites/fighter.png");
    defRoot->setObject("spriteSize", this->makeSpriteSize(64.f, 96.f));
    defRoot->setString("animations", "assets/animations/fighter_custom.json");
    defRoot->setString("stateMachine", "assets/fsm/fighter_custom.json");

    auto customStateNode = std::make_unique<StubDataNode>();
    customStateNode->setString("", "PowerCharge");
    std::vector<std::unique_ptr<DataNode>> customStates;
    customStates.push_back(std::move(customStateNode));
    defRoot->setArray("customStates", std::move(customStates));

    auto frame = std::make_unique<StubDataNode>();
    frame->setInt("x", 0);
    frame->setInt("y", 0);
    frame->setInt("width", 64);
    frame->setInt("height", 96);
    auto animCustom = std::make_unique<StubDataNode>();
    animCustom->setString("state", "PowerCharge");
    animCustom->setFloat("frameDuration", 0.2f);
    animCustom->setBool("loop", false);
    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    animCustom->setArray("frames", std::move(frames));
    auto animRoot = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> animations;
    animations.push_back(std::move(animCustom));
    animRoot->setArray("animations", std::move(animations));

    auto trans = std::make_unique<StubDataNode>();
    trans->setString("from", "Idle");
    trans->setString("to", "PowerCharge");
    trans->setString("trigger", "Punched");
    auto fsmRoot = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> transitions;
    transitions.push_back(std::move(trans));
    fsmRoot->setArray("transitions", std::move(transitions));

    auto clipRoot = this->makeEmptyClipsRoot();

    StubDataParser defParser, animParser, fsmParser, clipParser;
    defParser.registerNode("def.json", std::move(defRoot));
    animParser.registerNode("assets/animations/fighter_custom.json", std::move(animRoot));
    fsmParser.registerNode("assets/fsm/fighter_custom.json", std::move(fsmRoot));
    clipParser.registerNode("clip.json", std::move(clipRoot));

    CharacterDefinitionLoader defLoader(defParser);
    AnimationLoader animLoader(animParser);
    StateMachineLoader fsmLoader(fsmParser);
    CollisionClipLoader clipLoader(clipParser);

    StubResourceManager resourceManager;
    StubTextureFactory textureFactory;
    TextureLoader textureLoader(textureFactory);

    CharacterLoader loader(CharacterLoader::Config{
        .defLoader          = defLoader,
        .animLoader         = animLoader,
        .fsmLoader          = fsmLoader,
        .resourceManager    = resourceManager,
        .textureLoader      = textureLoader,
        .clipLoader         = clipLoader
    });

    World world;
    auto& components = world.components();
    components.registerComponent<SpriteComponent>();
    components.registerComponent<StateComponent>();
    components.registerComponent<StateMappingComponent>();
    components.registerComponent<StateMachineComponent>();
    components.registerComponent<AnimationControllerComponent>();
    components.registerComponent<AnimationComponent>();
    components.registerComponent<CollisionClipDefinitionsComponent>();
    components.registerComponent<CollisionClipPlayerComponent>();

    const auto entity = loader.create(world, "def.json");

    const auto& mapping = components.get<StateMappingComponent>(entity);
    REQUIRE(mapping.mapper != nullptr);
    const auto customStateId = mapping.mapper->fromString("PowerCharge");
    REQUIRE(customStateId.isCustom());

    const auto& stateMachine = components.get<StateMachineComponent>(entity);
    REQUIRE(stateMachine.machine.transitions.size() == 1);
    REQUIRE(stateMachine.machine.transitions[0].to == customStateId);

    const auto& controller = components.get<AnimationControllerComponent>(entity);
    REQUIRE(controller.animations.right.contains(customStateId));
}
