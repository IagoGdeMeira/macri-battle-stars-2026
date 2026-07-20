#include "game/include/AnimationLoader/AnimationLoader.h"

#include "StubDataNode.h"
#include "StubDataParser.h"

#include "game/include/StateIdMapper/StateIdMapper.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class AnimationLoaderFixture
{
public:
    StubDataParser parser;

    AnimationLoaderFixture() { this->parser.registerNode("assets/animations/fighter.json", this->makeAnimationsRoot()); }

    std::unique_ptr<StubDataNode> makeAnimationsRoot() const
    {
        auto frameA = std::make_unique<StubDataNode>();
        frameA->setInt("x", 0);
        frameA->setInt("y", 0);
        frameA->setInt("width", 16);
        frameA->setInt("height", 24);

        auto frameB = std::make_unique<StubDataNode>();
        frameB->setInt("x", 16);
        frameB->setInt("y", 0);
        frameB->setInt("width", 16);
        frameB->setInt("height", 24);

        auto idleAnimation = std::make_unique<StubDataNode>();
        idleAnimation->setString("state", "Idle");
        idleAnimation->setFloat("frameDuration", 0.08f);
        idleAnimation->setBool("loop", true);

        std::vector<std::unique_ptr<DataNode>> idleFrames;
        idleFrames.push_back(std::move(frameA));
        idleFrames.push_back(std::move(frameB));
        idleAnimation->setArray("frames", std::move(idleFrames));

        auto rootNode = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> animations;
        animations.push_back(std::move(idleAnimation));
        rootNode->setArray("animations", std::move(animations));

        return rootNode;
    }

    std::unique_ptr<StubDataNode> makeBadAnimationsRoot() const
    {
        auto animation = std::make_unique<StubDataNode>();
        animation->setString("state", "NonExistentState");
        animation->setFloat("frameDuration", 0.1f);
        animation->setBool("loop", true);

        auto frame = std::make_unique<StubDataNode>();
        frame->setInt("x", 0);
        frame->setInt("y", 0);
        frame->setInt("width", 16);
        frame->setInt("height", 16);

        std::vector<std::unique_ptr<DataNode>> frames;
        frames.push_back(std::move(frame));
        animation->setArray("frames", std::move(frames));

        auto rootNode = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> animations;
        animations.push_back(std::move(animation));
        rootNode->setArray("animations", std::move(animations));

        return rootNode;
    }
};

TEST_CASE_METHOD(AnimationLoaderFixture, "AnimationLoader parses mapped states and frames",
    "[unit][animation_loader]"
) {
    AnimationLoader loader(this->parser);
    const auto animations = loader.load("assets/animations/fighter.json");

    REQUIRE(animations.right.size() == 1);
    REQUIRE(animations.right.contains(StateId::Idle));

    const auto& idle = animations.right.at(StateId::Idle);
    REQUIRE(idle.frameDuration == 0.08f);
    REQUIRE(idle.loop == true);
    REQUIRE(idle.frames.size() == 2);
    REQUIRE(idle.frames[0].x == 0);
    REQUIRE(idle.frames[0].y == 0);
    REQUIRE(idle.frames[0].width == 16);
    REQUIRE(idle.frames[0].height == 24);
    REQUIRE(idle.frames[1].x == 16);
    REQUIRE(idle.frames[1].y == 0);
    REQUIRE(idle.frames[1].width == 16);
    REQUIRE(idle.frames[1].height == 24);
}

TEST_CASE_METHOD(AnimationLoaderFixture, "AnimationLoader rejects unknown state names",
    "[unit][animation_loader]"
) {
    StubDataParser localParser;
    localParser.registerNode("assets/animations/bad.json", this->makeBadAnimationsRoot());
    AnimationLoader loader(localParser);
    REQUIRE_THROWS(loader.load("assets/animations/bad.json"));
}

TEST_CASE_METHOD(AnimationLoaderFixture, "AnimationLoader resolves custom state names with mapper",
    "[unit][animation_loader]"
) {
    auto frame = std::make_unique<StubDataNode>();
    frame->setInt("x", 0);
    frame->setInt("y", 0);
    frame->setInt("width", 32);
    frame->setInt("height", 32);

    auto animation = std::make_unique<StubDataNode>();
    animation->setString("state", "PowerCharge");
    animation->setFloat("frameDuration", 0.2f);
    animation->setBool("loop", false);

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    animation->setArray("frames", std::move(frames));

    auto rootNode = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> animations;
    animations.push_back(std::move(animation));
    rootNode->setArray("animations", std::move(animations));

    StubDataParser localParser;
    localParser.registerNode("assets/animations/custom.json", std::move(rootNode));
    AnimationLoader loader(localParser);

    StateIdMapper mapper;
    const auto customState = mapper.addCustomMapping("PowerCharge");

    const auto loaded = loader.load("assets/animations/custom.json", mapper);

    REQUIRE(loaded.right.size() == 1);
    REQUIRE(loaded.right.contains(customState));
    REQUIRE(loaded.right.at(customState).frameDuration == 0.2f);
    REQUIRE(loaded.right.at(customState).loop == false);
}

TEST_CASE_METHOD(AnimationLoaderFixture, "AnimationLoader with mapper rejects unmapped custom state",
    "[unit][animation_loader]"
) {
    auto frame = std::make_unique<StubDataNode>();
    frame->setInt("x", 0);
    frame->setInt("y", 0);
    frame->setInt("width", 16);
    frame->setInt("height", 16);

    auto animation = std::make_unique<StubDataNode>();
    animation->setString("state", "PowerCharge");
    animation->setFloat("frameDuration", 0.1f);
    animation->setBool("loop", true);

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    animation->setArray("frames", std::move(frames));

    auto rootNode = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> animations;
    animations.push_back(std::move(animation));
    rootNode->setArray("animations", std::move(animations));

    StubDataParser localParser;
    localParser.registerNode("assets/animations/custom_bad.json", std::move(rootNode));
    AnimationLoader loader(localParser);

    StateIdMapper mapper;
    REQUIRE_THROWS_AS(loader.load("assets/animations/custom_bad.json", mapper), std::runtime_error);
}
