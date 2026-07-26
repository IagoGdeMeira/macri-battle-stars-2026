#include "game/include/PushboxLoader/PushboxLoader.h"

#include "StubAnimationLoader.h"
#include "StubDataParser.h"
#include "StubResourceManager.h"
#include "StubTextureFactory.h"
#include "StubTextureLoader.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/CircleColliderComponent.h"
#include "domain/components/PushboxComponent.h"
#include "domain/components/PushboxControllerComponent.h"
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

class PushboxLoaderFixture
{
public:
    PushboxLoaderFixture() :
        textureLoader(this->textureFactory),
        factory(std::make_unique<EntityFactory>(EntityFactory::Config{
            this->world, this->resourceManager, this->textureLoader, this->animLoader})),
        loader(std::make_unique<PushboxLoader>(this->parser, *this->factory))
    {
        auto& comp = this->world.components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<RectangleColliderComponent>();
        comp.registerComponent<CircleColliderComponent>();
        comp.registerComponent<PushboxComponent>();
        comp.registerComponent<ActiveComponent>();
        comp.registerComponent<PushboxControllerComponent>();
    }

    World world;
    StubDataParser parser;
    StubResourceManager resourceManager;
    StubTextureLoader textureLoader;
    StubTextureFactory textureFactory;
    StubAnimationLoader animLoader;
    std::unique_ptr<EntityFactory> factory;
    std::unique_ptr<PushboxLoader> loader;

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

    std::unique_ptr<StubDataNode> createPushboxNode(
        const std::string& type,
        float x, float y,
        float width, float height,
        const std::string& pushboxType,
        float mass,
        float pushResistance
    ) {
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
        
        node->setString("pushboxType", pushboxType);
        node->setFloat("mass", mass);
        node->setFloat("pushResistance", pushResistance);
        return node;
    }
};

TEST_CASE_METHOD(PushboxLoaderFixture, "PushboxLoader loads a single state with rectangle pushbox", "[unit][pushbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", false);
    auto frame = this->createFrameNode(0.1f);
    auto pushbox = this->createPushboxNode("rectangle", 10.f, 5.f, 20.f, 30.f, "static", 100.f, 0.5f);

    std::vector<std::unique_ptr<DataNode>> pushboxes;
    pushboxes.push_back(std::move(pushbox));
    frame->setArray("pushboxes", std::move(pushboxes));

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->load(*root, parent, false);

    REQUIRE(controllerMap.size() == 1);
    auto it = controllerMap.find(StateId::Idle);
    REQUIRE(it != controllerMap.end());

    const auto& controller = it->second;
    REQUIRE(controller.frames.size() == 1);
    REQUIRE(controller.frames[0].duration == 0.1f);
    REQUIRE(controller.frames[0].pushboxes.size() == 1);
    REQUIRE(controller.loop == false);

    Entity pushboxEntity = controller.frames[0].pushboxes[0];
    auto& comp = this->world.components();
    REQUIRE(comp.has<PushboxComponent>(pushboxEntity));
    const auto& pb = comp.get<PushboxComponent>(pushboxEntity);
    REQUIRE(pb.type == PushboxComponent::Type::Static);
    REQUIRE(pb.mass == 100.f);
    REQUIRE(pb.pushResistance == 0.5f);
    REQUIRE(comp.has<RectangleColliderComponent>(pushboxEntity));
}

TEST_CASE_METHOD(PushboxLoaderFixture, "PushboxLoader loads a single state with circle pushbox", "[unit][pushbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", false);
    auto frame = this->createFrameNode(0.15f);
    auto pushbox = this->createPushboxNode("circle", 0.f, 0.f, 15.f, 0.f, "dynamic", 50.f, 0.8f);

    std::vector<std::unique_ptr<DataNode>> pushboxes;
    pushboxes.push_back(std::move(pushbox));
    frame->setArray("pushboxes", std::move(pushboxes));

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->load(*root, parent, false);

    REQUIRE(controllerMap.size() == 1);
    auto it = controllerMap.find(StateId::Idle);
    REQUIRE(it != controllerMap.end());

    const auto& controller = it->second;
    REQUIRE(controller.frames.size() == 1);
    REQUIRE(controller.frames[0].pushboxes.size() == 1);

    Entity pushboxEntity = controller.frames[0].pushboxes[0];
    auto& comp = this->world.components();
    REQUIRE(comp.has<PushboxComponent>(pushboxEntity));
    const auto& pb = comp.get<PushboxComponent>(pushboxEntity);
    REQUIRE(pb.type == PushboxComponent::Type::Dynamic);
    REQUIRE(pb.mass == 50.f);
    REQUIRE(pb.pushResistance == 0.8f);
    REQUIRE(comp.has<CircleColliderComponent>(pushboxEntity));
}

TEST_CASE_METHOD(PushboxLoaderFixture, "PushboxLoader handles multiple frames in a state", "[unit][pushbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", true);

    std::vector<std::unique_ptr<DataNode>> frames;
    for (int i = 0; i < 3; ++i)
    {
        auto frame = this->createFrameNode(0.05f);
        frame->setArray("pushboxes", {});
        frames.push_back(std::move(frame));
    }
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->load(*root, parent, false);

    REQUIRE(controllerMap.size() == 1);
    auto it = controllerMap.find(StateId::Idle);
    REQUIRE(it != controllerMap.end());

    const auto& controller = it->second;
    REQUIRE(controller.frames.size() == 3);
    REQUIRE(controller.loop == true);
    for (const auto& frame : controller.frames) REQUIRE(frame.duration == 0.05f);
}

TEST_CASE_METHOD(PushboxLoaderFixture, "PushboxLoader handles multiple states", "[unit][pushbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    std::vector<std::unique_ptr<DataNode>> states;

    auto idleState = this->createStateNode("Idle", false);
    auto idleFrame = this->createFrameNode(0.2f);
    idleFrame->setArray("pushboxes", {});
    std::vector<std::unique_ptr<DataNode>> idleFrames;
    idleFrames.push_back(std::move(idleFrame));
    idleState->setArray("frames", std::move(idleFrames));
    states.push_back(std::move(idleState));

    auto punchState = this->createStateNode("Punching", false);
    auto punchFrame = this->createFrameNode(0.1f);
    auto pushbox = this->createPushboxNode("rectangle", 10.f, 0.f, 20.f, 20.f, "dynamic", 60.f, 0.6f);
    std::vector<std::unique_ptr<DataNode>> punchPushboxes;
    punchPushboxes.push_back(std::move(pushbox));
    punchFrame->setArray("pushboxes", std::move(punchPushboxes));
    std::vector<std::unique_ptr<DataNode>> punchFrames;
    punchFrames.push_back(std::move(punchFrame));
    punchState->setArray("frames", std::move(punchFrames));
    states.push_back(std::move(punchState));

    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->load(*root, parent, false);

    REQUIRE(controllerMap.size() == 2);
    REQUIRE(controllerMap.find(StateId::Idle) != controllerMap.end());
    REQUIRE(controllerMap.find(StateId::Punching) != controllerMap.end());

    const auto& idleController = controllerMap[StateId::Idle];
    REQUIRE(idleController.frames.size() == 1);
    REQUIRE(idleController.frames[0].pushboxes.empty());

    const auto& punchController = controllerMap[StateId::Punching];
    REQUIRE(punchController.frames.size() == 1);
    REQUIRE(punchController.frames[0].pushboxes.size() == 1);
}

TEST_CASE_METHOD(PushboxLoaderFixture, "PushboxLoader throws on unknown pushbox type", "[unit][pushbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", false);
    auto frame = this->createFrameNode(0.1f);

    auto pushbox = std::make_unique<StubDataNode>();
    pushbox->setString("type", "polygon");
    pushbox->setFloat("x", 0.f);
    pushbox->setFloat("y", 0.f);

    std::vector<std::unique_ptr<DataNode>> pushboxes;
    pushboxes.push_back(std::move(pushbox));
    frame->setArray("pushboxes", std::move(pushboxes));

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    REQUIRE_THROWS_AS(this->loader->load(*root, parent, false), std::runtime_error);
}

TEST_CASE_METHOD(PushboxLoaderFixture, "PushboxLoader handles empty pushbox list", "[unit][pushbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", false);
    auto frame = this->createFrameNode(0.2f);
    frame->setArray("pushboxes", {});

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->load(*root, parent, false);

    REQUIRE(controllerMap.size() == 1);
    auto it = controllerMap.find(StateId::Idle);
    REQUIRE(it != controllerMap.end());

    const auto& controller = it->second;
    REQUIRE(controller.frames.size() == 1);
    REQUIRE(controller.frames[0].pushboxes.empty());
}
