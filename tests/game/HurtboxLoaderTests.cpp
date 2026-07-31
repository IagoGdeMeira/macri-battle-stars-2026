#include "game/include/HurtboxLoader/HurtboxLoader.h"

#include "StubAnimationLoader.h"
#include "StubDataParser.h"
#include "StubResourceManager.h"
#include "StubTextureFactory.h"
#include "StubTextureLoader.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/CircleColliderComponent.h"
#include "domain/components/HurtboxComponent.h"
#include "domain/components/HurtboxControllerComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/DataNode/DataNode.h"
#include "engine/include/DataParser/DataParser.h"

#include "game/include/EntityFactory/EntityFactory.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <stdexcept>

class HurtboxLoaderFixture
{
public:
    HurtboxLoaderFixture() :
        textureFactory(),
        textureLoader(this->textureFactory),
        factory(std::make_unique<EntityFactory>(EntityFactory::Config{
            this->world, this->resourceManager, this->textureLoader, this->animLoader})),
        loader(std::make_unique<HurtboxLoader>(this->parser, *this->factory))
    {
        auto& comp = this->world.components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<RectangleColliderComponent>();
        comp.registerComponent<CircleColliderComponent>();
        comp.registerComponent<HurtboxComponent>();
        comp.registerComponent<ActiveComponent>();
        comp.registerComponent<HurtboxControllerComponent>();
    }

    World world;
    StubDataParser parser;
    StubResourceManager resourceManager;
    StubTextureFactory textureFactory;
    StubTextureLoader textureLoader;
    StubAnimationLoader animLoader;
    std::unique_ptr<EntityFactory> factory;
    std::unique_ptr<HurtboxLoader> loader;

    std::unique_ptr<StubDataNode> createStateNode(const std::string& name, bool loop)
    {
        auto state = std::make_unique<StubDataNode>();
        state->setString("name", name);
        state->setBool("loop", loop);
        return state;
    }

    std::unique_ptr<StubDataNode> createFrameNode(float duration)
    {
        auto frame = std::make_unique<StubDataNode>();
        frame->setFloat("duration", duration);
        return frame;
    }

    std::unique_ptr<StubDataNode> createHurtboxNode(const std::string& type, float x, float y, float width, float height, float damageMultiplier)
    {
        auto node = std::make_unique<StubDataNode>();
        node->setString("type", type);
        node->setFloat("x", x);
        node->setFloat("y", y);
        if (type == "rectangle")
        {
            node->setFloat("width", width);
            node->setFloat("height", height);
        }
        else if (type == "circle") node->setFloat("radius", width);
        node->setFloat("damageMultiplier", damageMultiplier);
        return node;
    }
};

TEST_CASE_METHOD(HurtboxLoaderFixture, "HurtboxLoader loads a single state with rectangle hurtbox", "[unit][hurtbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", false);
    auto frame = this->createFrameNode(0.1f);
    auto hurtbox = this->createHurtboxNode("rectangle", 10.f, 5.f, 20.f, 30.f, 1.5f);

    std::vector<std::unique_ptr<DataNode>> hurtboxes;
    hurtboxes.push_back(std::move(hurtbox));
    frame->setArray("hurtboxes", std::move(hurtboxes));

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->loadSingleState(*root, parent);

    REQUIRE(controllerMap.size() == 1);
    auto it = controllerMap.find(StateId::Idle);
    REQUIRE(it != controllerMap.end());

    const auto& controller = it->second;
    REQUIRE(controller.frames.size() == 1);
    REQUIRE(controller.frames[0].duration == 0.1f);
    REQUIRE(controller.frames[0].hurtboxes.size() == 1);
    REQUIRE(controller.loop == false);

    Entity hurtboxEntity = controller.frames[0].hurtboxes[0];
    auto& comp = this->world.components();
    REQUIRE(comp.has<HurtboxComponent>(hurtboxEntity));
    REQUIRE(comp.get<HurtboxComponent>(hurtboxEntity).damageMultiplier == 1.5f);
    REQUIRE(comp.has<RectangleColliderComponent>(hurtboxEntity));
}

TEST_CASE_METHOD(HurtboxLoaderFixture, "HurtboxLoader loads a single state with circle hurtbox", "[unit][hurtbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", false);
    auto frame = this->createFrameNode(0.15f);
    auto hurtbox = this->createHurtboxNode("circle", 0.f, 0.f, 15.f, 0.f, 2.f);

    std::vector<std::unique_ptr<DataNode>> hurtboxes;
    hurtboxes.push_back(std::move(hurtbox));
    frame->setArray("hurtboxes", std::move(hurtboxes));

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->loadSingleState(*root, parent);

    REQUIRE(controllerMap.size() == 1);
    auto it = controllerMap.find(StateId::Idle);
    REQUIRE(it != controllerMap.end());

    const auto& controller = it->second;
    REQUIRE(controller.frames.size() == 1);
    REQUIRE(controller.frames[0].hurtboxes.size() == 1);

    Entity hurtboxEntity = controller.frames[0].hurtboxes[0];
    auto& comp = this->world.components();
    REQUIRE(comp.has<HurtboxComponent>(hurtboxEntity));
    REQUIRE(comp.get<HurtboxComponent>(hurtboxEntity).damageMultiplier == 2.f);
    REQUIRE(comp.has<CircleColliderComponent>(hurtboxEntity));
}

TEST_CASE_METHOD(HurtboxLoaderFixture, "HurtboxLoader handles multiple frames in a state", "[unit][hurtbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", true);

    std::vector<std::unique_ptr<DataNode>> frames;
    for (int i = 0; i < 3; ++i)
    {
        auto frame = this->createFrameNode(0.05f);
        frame->setArray("hurtboxes", {});
        frames.push_back(std::move(frame));
    }
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->loadSingleState(*root, parent);

    REQUIRE(controllerMap.size() == 1);
    auto it = controllerMap.find(StateId::Idle);
    REQUIRE(it != controllerMap.end());

    const auto& controller = it->second;
    REQUIRE(controller.frames.size() == 3);
    REQUIRE(controller.loop == true);
    for (const auto& frame : controller.frames) REQUIRE(frame.duration == 0.05f);
}

TEST_CASE_METHOD(HurtboxLoaderFixture, "HurtboxLoader handles multiple states", "[unit][hurtbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    std::vector<std::unique_ptr<DataNode>> states;

    auto idleState = this->createStateNode("Idle", false);
    auto idleFrame = this->createFrameNode(0.2f);
    idleFrame->setArray("hurtboxes", {});
    std::vector<std::unique_ptr<DataNode>> idleFrames;
    idleFrames.push_back(std::move(idleFrame));
    idleState->setArray("frames", std::move(idleFrames));
    states.push_back(std::move(idleState));

    auto punchState = this->createStateNode("Punching", false);
    auto punchFrame = this->createFrameNode(0.1f);
    auto hurtbox = this->createHurtboxNode("rectangle", 10.f, 0.f, 20.f, 20.f, 1.f);
    std::vector<std::unique_ptr<DataNode>> punchHurtboxes;
    punchHurtboxes.push_back(std::move(hurtbox));
    punchFrame->setArray("hurtboxes", std::move(punchHurtboxes));
    std::vector<std::unique_ptr<DataNode>> punchFrames;
    punchFrames.push_back(std::move(punchFrame));
    punchState->setArray("frames", std::move(punchFrames));
    states.push_back(std::move(punchState));

    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->loadSingleState(*root, parent);

    REQUIRE(controllerMap.size() == 2);
    REQUIRE(controllerMap.find(StateId::Idle) != controllerMap.end());
    REQUIRE(controllerMap.find(StateId::Punching) != controllerMap.end());

    const auto& idleController = controllerMap[StateId::Idle];
    REQUIRE(idleController.frames.size() == 1);
    REQUIRE(idleController.frames[0].hurtboxes.empty());

    const auto& punchController = controllerMap[StateId::Punching];
    REQUIRE(punchController.frames.size() == 1);
    REQUIRE(punchController.frames[0].hurtboxes.size() == 1);
}

TEST_CASE_METHOD(HurtboxLoaderFixture, "HurtboxLoader throws on unknown hurtbox type", "[unit][hurtbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", false);
    auto frame = this->createFrameNode(0.1f);

    auto hurtbox = std::make_unique<StubDataNode>();
    hurtbox->setString("type", "polygon");
    hurtbox->setFloat("x", 0.f);
    hurtbox->setFloat("y", 0.f);

    std::vector<std::unique_ptr<DataNode>> hurtboxes;
    hurtboxes.push_back(std::move(hurtbox));
    frame->setArray("hurtboxes", std::move(hurtboxes));

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    REQUIRE_THROWS_AS(this->loader->loadSingleState(*root, parent), std::runtime_error);
}

TEST_CASE_METHOD(HurtboxLoaderFixture, "HurtboxLoader handles empty hurtbox list", "[unit][hurtbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", false);
    auto frame = this->createFrameNode(0.2f);
    frame->setArray("hurtboxes", {});

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->loadSingleState(*root, parent);

    REQUIRE(controllerMap.size() == 1);
    auto it = controllerMap.find(StateId::Idle);
    REQUIRE(it != controllerMap.end());

    const auto& controller = it->second;
    REQUIRE(controller.frames.size() == 1);
    REQUIRE(controller.frames[0].hurtboxes.empty());
}
