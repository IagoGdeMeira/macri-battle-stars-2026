#include "../../src/game/include/CharacterLoader/CharacterLoader.h"

#include "../../src/domain/components/AnimationComponent.h"
#include "../../src/domain/components/AnimationControllerComponent.h"
#include "../../src/domain/components/SpriteComponent.h"
#include "../../src/domain/components/StateComponent.h"
#include "../../src/domain/components/StateMappingComponent.h"
#include "../../src/domain/components/StateMachineComponent.h"
#include "../../src/domain/include/StateId/StateId.h"
#include "../../src/domain/include/TriggerId/TriggerId.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/DataNode/DataNode.h"
#include "../../src/engine/include/DataParser/DataParser.h"
#include "../../src/engine/include/Renderer/Renderer.h"
#include "../../src/engine/include/ResourceManager/ResourceManager.h"
#include "../../src/engine/include/ThreadPool/ThreadPool.h"

#include "../../src/game/include/AnimationLoader/AnimationLoader.h"
#include "../../src/game/include/CharacterDefinitionLoader/CharacterDefinitionLoader.h"
#include "../../src/game/include/StateIdMapper/StateIdMapper.h"
#include "../../src/game/include/StateMachineLoader/StateMachineLoader.h"
#include "../../src/game/include/TextureLoader/TextureLoader.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class CharacterLoaderFixture
{
public:
    class Node : public DataNode
    {
    public:
        void setString(const std::string& key, const std::string& value) { this->strings[key] = value; }
        void setInt(const std::string& key, int value) { this->ints[key] = value; }
        void setFloat(const std::string& key, float value) { this->floats[key] = value; }
        void setBool(const std::string& key, bool value) { this->bools[key] = value; }

        void setArray(const std::string& key, std::vector<std::unique_ptr<DataNode>> value)
        {
            std::vector<Node> converted;
            converted.reserve(value.size());

            for (auto& node : value)
            {
                auto* typed = dynamic_cast<Node*>(node.get());
                if (!typed) throw std::runtime_error("Unexpected node type in fake tree");
                converted.push_back(*typed);
            }

            arrays[key] = std::move(converted);
        }

        bool has(const std::string& key) const override
        {
            return this->strings.contains(key)
                || this->ints.contains(key)
                || this->floats.contains(key)
                || this->bools.contains(key)
                || this->arrays.contains(key);
        }

        std::string getString(const std::string& key, const std::string& fallback = DataNode::defaultStringFallback) const override
        {
            const auto it = this->strings.find(key);
            if (it == this->strings.end()) return fallback;
            return it->second;
        }

        int getInt(const std::string& key, const int& fallback = DataNode::defaultIntFallback) const override
        {
            const auto it = this->ints.find(key);
            if (it == this->ints.end()) return fallback;
            return it->second;
        }

        float getFloat(const std::string& key, const float& fallback = DataNode::defaultFloatFallback) const override
        {
            const auto it = this->floats.find(key);
            if (it == this->floats.end()) return fallback;
            return it->second;
        }

        bool getBool(const std::string& key, const bool& fallback = DataNode::defaultBoolFallback) const override
        {
            const auto it = this->bools.find(key);
            if (it == this->bools.end()) return fallback;
            return it->second;
        }

        std::vector<std::unique_ptr<DataNode>> getArray(const std::string& key) const override
        {
            const auto it = this->arrays.find(key);
            if (it == this->arrays.end()) throw std::runtime_error("Missing array key: " + key);

            std::vector<std::unique_ptr<DataNode>> out;
            out.reserve(it->second.size());

            for (const auto& node : it->second)
            { out.push_back(std::make_unique<Node>(node)); }

            return out;
        }

    private:
        std::unordered_map<std::string, std::string> strings;
        std::unordered_map<std::string, int> ints;
        std::unordered_map<std::string, float> floats;
        std::unordered_map<std::string, bool> bools;
        std::unordered_map<std::string, std::vector<Node>> arrays;
    };

    class Parser : public DataParser
    {
    public:
        explicit Parser(std::unique_ptr<DataNode> rootNode) : root(std::move(rootNode)) {}

        mutable std::string lastPath;

        std::unique_ptr<DataNode> parse(const std::string& filePath) const override
        {
            this->lastPath = filePath;

            auto* typed = dynamic_cast<Node*>(root.get());
            if (!typed) throw std::runtime_error("Unexpected root node type in fake parser");

            return std::make_unique<Node>(*typed);
        }

    private:
        std::unique_ptr<DataNode> root;
    };

    class StubRenderer : public Renderer
    {
    public:
        int createTextureCalls = 0;
        std::string lastTexturePath;
        std::shared_ptr<Texture> textureToReturn = std::make_shared<Texture>();

        void clear() override {}
        void present() override {}

        std::shared_ptr<Texture> createTexture(const std::string& filePath) override
        {
            this->createTextureCalls++;
            this->lastTexturePath = filePath;
            return this->textureToReturn;
        }

        void drawTexture(const Texture& texture, const Renderer::DrawTextureParams& params) override
        { (void)texture; (void)params; }

        void drawRectOutline(const Rectangle& rect, const Renderer::Color& color) override
        { (void)rect; (void)color; }

        void drawRectFilled(const Rectangle& rect, const Renderer::Color& color) override
        { (void)rect; (void)color; }

        void drawCircleOutline(const Circle& circle, const Renderer::Color& color) override
        { (void)circle; (void)color; }

        void drawCircleFilled(const Circle& circle, const Renderer::Color& color) override
        { (void)circle; (void)color; }

        void setViewport(const Viewport& viewport) override
        { (void)viewport; }
    };

    CharacterLoaderFixture() : resourceManager(threadPool)
    {
        this->world.components().registerComponent<SpriteComponent>();
        this->world.components().registerComponent<StateComponent>();
        this->world.components().registerComponent<StateMappingComponent>();
        this->world.components().registerComponent<StateMachineComponent>();
        this->world.components().registerComponent<AnimationControllerComponent>();
        this->world.components().registerComponent<AnimationComponent>();
    }

    std::unique_ptr<DataNode> makeDefinitionRoot() const
    {
        auto rootNode = std::make_unique<Node>();
        rootNode->setString("id", "fighter_01");
        rootNode->setString("texture", "assets/sprites/fighter.png");
        rootNode->setInt("spriteWidth", 64);
        rootNode->setInt("spriteHeight", 96);
        rootNode->setString("animations", "assets/animations/fighter_01.json");
        rootNode->setString("stateMachine", "assets/fsm/fighter_01.json");
        return rootNode;
    }

    std::unique_ptr<DataNode> makeAnimationRoot() const
    {
        auto frameA = std::make_unique<Node>();
        frameA->setInt("x", 0);
        frameA->setInt("y", 0);
        frameA->setInt("width", 64);
        frameA->setInt("height", 96);

        auto animation = std::make_unique<Node>();
        animation->setString("state", "Idle");
        animation->setFloat("frameDuration", 0.12f);
        animation->setBool("loop", true);

        std::vector<std::unique_ptr<DataNode>> frames;
        frames.push_back(std::move(frameA));
        animation->setArray("frames", std::move(frames));

        auto rootNode = std::make_unique<Node>();
        std::vector<std::unique_ptr<DataNode>> animations;
        animations.push_back(std::move(animation));
        rootNode->setArray("animations", std::move(animations));

        return rootNode;
    }

    std::unique_ptr<DataNode> makeStateMachineRoot() const
    {
        auto transition = std::make_unique<Node>();
        transition->setString("from", "Idle");
        transition->setString("to", "Punching");
        transition->setString("trigger", "Punched");

        auto rootNode = std::make_unique<Node>();
        std::vector<std::unique_ptr<DataNode>> transitions;
        transitions.push_back(std::move(transition));
        rootNode->setArray("transitions", std::move(transitions));

        return rootNode;
    }

    World world;
    ThreadPool threadPool { 1 };
    ResourceManager resourceManager;
    StubRenderer renderer;
};

TEST_CASE_METHOD(CharacterLoaderFixture, "CharacterLoader creates entity and required gameplay components",
    "[integration][character_loader]"
) {
    Parser definitionParser(this->makeDefinitionRoot());
    Parser animationParser(this->makeAnimationRoot());
    Parser stateMachineParser(this->makeStateMachineRoot());

    CharacterDefinitionLoader definitionLoader(definitionParser);
    AnimationLoader animationLoader(animationParser);
    StateMachineLoader machineLoader(stateMachineParser);
    TextureLoader textureLoader(renderer);

    CharacterLoader loader(
        definitionLoader,
        animationLoader,
        machineLoader,
        resourceManager,
        textureLoader
    );

    const auto entity = loader.create(this->world, "assets/characters/fighter_01.json");

    REQUIRE(definitionParser.lastPath == "assets/characters/fighter_01.json");
    REQUIRE(animationParser.lastPath == "assets/animations/fighter_01.json");
    REQUIRE(stateMachineParser.lastPath == "assets/fsm/fighter_01.json");
    REQUIRE(renderer.createTextureCalls == 1);
    REQUIRE(renderer.lastTexturePath == "assets/sprites/fighter.png");

    REQUIRE(this->world.components().has<SpriteComponent>(entity));
    REQUIRE(this->world.components().has<StateComponent>(entity));
    REQUIRE(this->world.components().has<StateMappingComponent>(entity));
    REQUIRE(this->world.components().has<StateMachineComponent>(entity));
    REQUIRE(this->world.components().has<AnimationControllerComponent>(entity));
    REQUIRE(this->world.components().has<AnimationComponent>(entity));

    const auto& sprite = this->world.components().get<SpriteComponent>(entity);
    const auto& state = this->world.components().get<StateComponent>(entity);
    const auto& stateMachine = this->world.components().get<StateMachineComponent>(entity);
    const auto& controller = this->world.components().get<AnimationControllerComponent>(entity);
    const auto& animation = this->world.components().get<AnimationComponent>(entity);

    REQUIRE(sprite.texture == renderer.textureToReturn);
    REQUIRE(sprite.width == 64);
    REQUIRE(sprite.height == 96);
    REQUIRE(sprite.useSourceRect == false);

    REQUIRE(state.current == StateId::Idle);
    REQUIRE(state.timeInState == 0.0f);

    REQUIRE(stateMachine.machine.transitions.size() == 1);
    REQUIRE(stateMachine.machine.transitions[0].from == StateId::Idle);
    REQUIRE(stateMachine.machine.transitions[0].to == StateId::Punching);

    REQUIRE(controller.animations.size() == 1);
    REQUIRE(controller.animations.contains(StateId::Idle));
    REQUIRE(controller.currentState == StateId::Unknown);

    REQUIRE(animation.currentFrame == 0);
    REQUIRE(animation.elapsedTime == 0.0f);
    REQUIRE(animation.currentState == StateId::Unknown);
}

TEST_CASE_METHOD(CharacterLoaderFixture, "CharacterLoader resolves custom states per character definition",
    "[integration][character_loader]"
) {
    auto definitionRoot = std::make_unique<Node>();
    definitionRoot->setString("id", "fighter_custom");
    definitionRoot->setString("texture", "assets/sprites/fighter.png");
    definitionRoot->setInt("spriteWidth", 64);
    definitionRoot->setInt("spriteHeight", 96);
    definitionRoot->setString("animations", "assets/animations/fighter_custom.json");
    definitionRoot->setString("stateMachine", "assets/fsm/fighter_custom.json");

    auto customState = std::make_unique<Node>();
    customState->setString("", "PowerCharge");
    std::vector<std::unique_ptr<DataNode>> customStates;
    customStates.push_back(std::move(customState));
    definitionRoot->setArray("customStates", std::move(customStates));

    auto frame = std::make_unique<Node>();
    frame->setInt("x", 0);
    frame->setInt("y", 0);
    frame->setInt("width", 64);
    frame->setInt("height", 96);

    auto customAnimation = std::make_unique<Node>();
    customAnimation->setString("state", "PowerCharge");
    customAnimation->setFloat("frameDuration", 0.2f);
    customAnimation->setBool("loop", false);

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    customAnimation->setArray("frames", std::move(frames));

    auto animationRoot = std::make_unique<Node>();
    std::vector<std::unique_ptr<DataNode>> animations;
    animations.push_back(std::move(customAnimation));
    animationRoot->setArray("animations", std::move(animations));

    auto transition = std::make_unique<Node>();
    transition->setString("from", "Idle");
    transition->setString("to", "PowerCharge");
    transition->setString("trigger", "Punched");

    auto machineRoot = std::make_unique<Node>();
    std::vector<std::unique_ptr<DataNode>> transitions;
    transitions.push_back(std::move(transition));
    machineRoot->setArray("transitions", std::move(transitions));

    Parser definitionParser(std::move(definitionRoot));
    Parser animationParser(std::move(animationRoot));
    Parser stateMachineParser(std::move(machineRoot));

    CharacterDefinitionLoader definitionLoader(definitionParser);
    AnimationLoader animationLoader(animationParser);
    StateMachineLoader machineLoader(stateMachineParser);
    TextureLoader textureLoader(renderer);

    CharacterLoader loader(
        definitionLoader,
        animationLoader,
        machineLoader,
        resourceManager,
        textureLoader
    );

    const auto entity = loader.create(this->world, "assets/characters/fighter_custom.json");

    const auto& mapping = this->world.components().get<StateMappingComponent>(entity);
    REQUIRE(mapping.mapper != nullptr);

    const auto customStateId = mapping.mapper->fromString("PowerCharge");
    REQUIRE(customStateId.isCustom());

    const auto& stateMachine = this->world.components().get<StateMachineComponent>(entity);
    REQUIRE(stateMachine.machine.transitions.size() == 1);
    REQUIRE(stateMachine.machine.transitions[0].to == customStateId);

    const auto& controller = this->world.components().get<AnimationControllerComponent>(entity);
    REQUIRE(controller.animations.contains(customStateId));
}
