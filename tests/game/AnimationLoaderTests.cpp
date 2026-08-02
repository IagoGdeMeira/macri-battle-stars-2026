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

    void registerIndexEntry(
        const std::string& indexPath, const std::string& stateName, const std::string& stateFilePath, std::unique_ptr<StubDataNode> animNode
    ) {
        auto indexRoot = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> statesArray;

        auto entry = std::make_unique<StubDataNode>();
        entry->setString("name", stateName);
        entry->setString("path", stateFilePath);
        statesArray.push_back(std::move(entry));

        indexRoot->setArray("states", std::move(statesArray));
        this->parser.registerNode(indexPath, std::move(indexRoot));
        this->parser.registerNode(stateFilePath, std::move(animNode));
    }

    void registerIndexMultiple(
        const std::string& indexPath, std::vector<std::tuple<std::string, std::string, std::unique_ptr<StubDataNode>>> entries
    ) {
        auto indexRoot = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> statesArray;

        for (auto& [stateName, filePath, animNode] : entries)
        {
            auto entry = std::make_unique<StubDataNode>();
            entry->setString("name", stateName);
            entry->setString("path", filePath);
            statesArray.push_back(std::move(entry));
            this->parser.registerNode(filePath, std::move(animNode));
        }

        indexRoot->setArray("states", std::move(statesArray));
        this->parser.registerNode(indexPath, std::move(indexRoot));
    }

    std::unique_ptr<StubDataNode> makeAnimationNode(
        float frameDuration, bool loop, std::vector<std::unique_ptr<DataNode>> frames
    ) {
        auto animNode = std::make_unique<StubDataNode>();
        animNode->setFloat("frameDuration", frameDuration);
        animNode->setBool("loop", loop);
        animNode->setArray("frames", std::move(frames));
        return animNode;
    }

    std::unique_ptr<StubDataNode> makeFrame(int x, int y, int w, int h)
    {
        auto frame = std::make_unique<StubDataNode>();
        frame->setInt("x", x);
        frame->setInt("y", y);
        frame->setInt("width", w);
        frame->setInt("height", h);
        return frame;
    }
};

TEST_CASE_METHOD(AnimationLoaderFixture, "loadFromIndex loads a single state from an index",
    "[unit][animation_loader]")
{
    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(this->makeFrame(0, 0, 16, 24));
    frames.push_back(this->makeFrame(16, 0, 16, 24));
    auto animNode = this->makeAnimationNode(0.08f, true, std::move(frames));

    this->registerIndexEntry("assets/animations/index.json", "Idle", "idle.json", std::move(animNode));

    AnimationLoader loader(this->parser);
    StateIdMapper mapper;
    auto set = loader.loadFromIndex("assets/animations/index.json", mapper);

    REQUIRE(set.right.size() == 1);
    REQUIRE(set.right.contains(StateId::Idle));

    const auto& idle = set.right.at(StateId::Idle);
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

TEST_CASE_METHOD(AnimationLoaderFixture, "loadFromIndex handles multiple states",
    "[unit][animation_loader]")
{
    std::vector<std::unique_ptr<DataNode>> idleFrames;
    idleFrames.push_back(this->makeFrame(0, 0, 10, 10));
    auto idleAnim = this->makeAnimationNode(0.1f, false, std::move(idleFrames));

    std::vector<std::unique_ptr<DataNode>> runFrames;
    runFrames.push_back(this->makeFrame(10, 0, 20, 20));
    auto runAnim = this->makeAnimationNode(0.15f, true, std::move(runFrames));

    std::vector<std::tuple<std::string, std::string, std::unique_ptr<StubDataNode>>> entries;
    entries.emplace_back("Idle", "idle.json", std::move(idleAnim));
    entries.emplace_back("Running", "run.json", std::move(runAnim));

    this->registerIndexMultiple("index.json", std::move(entries));

    AnimationLoader loader(this->parser);
    StateIdMapper mapper;
    auto set = loader.loadFromIndex("index.json", mapper);

    REQUIRE(set.right.size() == 2);
    REQUIRE(set.right.contains(StateId::Idle));
    REQUIRE(set.right.contains(StateId::Running));
    REQUIRE(set.right.at(StateId::Idle).frames.size() == 1);
    REQUIRE(set.right.at(StateId::Running).frames.size() == 1);
    REQUIRE(set.symmetric == true);
}

TEST_CASE_METHOD(AnimationLoaderFixture, "loadFromIndex throws on unknown state",
    "[unit][animation_loader]")
{
    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(this->makeFrame(0, 0, 16, 16));
    auto animNode = this->makeAnimationNode(0.05f, false, std::move(frames));

    this->registerIndexEntry("index.json", "NonExistentState", "bad.json", std::move(animNode));

    AnimationLoader loader(this->parser);
    StateIdMapper mapper;
    REQUIRE_THROWS_AS(loader.loadFromIndex("index.json", mapper), std::runtime_error);
}

TEST_CASE_METHOD(AnimationLoaderFixture, "loadFromIndex resolves custom state names with mapper",
    "[unit][animation_loader]")
{
    StateIdMapper mapper;
    const auto powerChargeId = mapper.addCustomMapping("PowerCharge");

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(this->makeFrame(0, 0, 32, 32));
    auto animNode = this->makeAnimationNode(0.2f, false, std::move(frames));

    this->registerIndexEntry("index.json", "PowerCharge", "power_charge.json", std::move(animNode));

    AnimationLoader loader(this->parser);
    auto set = loader.loadFromIndex("index.json", mapper);

    REQUIRE(set.right.size() == 1);
    REQUIRE(set.right.contains(powerChargeId));
    REQUIRE(set.right.at(powerChargeId).frameDuration == 0.2f);
    REQUIRE(set.right.at(powerChargeId).loop == false);
}

TEST_CASE_METHOD(AnimationLoaderFixture, "loadFromIndex with mapper throws on unmapped custom state",
    "[unit][animation_loader]")
{
    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(this->makeFrame(0, 0, 16, 16));
    auto animNode = this->makeAnimationNode(0.1f, true, std::move(frames));

    this->registerIndexEntry("index.json", "PowerCharge", "power_charge.json", std::move(animNode));

    AnimationLoader loader(this->parser);
    StateIdMapper mapper;
    REQUIRE_THROWS_AS(loader.loadFromIndex("index.json", mapper), std::runtime_error);
}
