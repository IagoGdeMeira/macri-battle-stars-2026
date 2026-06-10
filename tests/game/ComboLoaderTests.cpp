#include "../../src/game/include/ComboLoader/ComboLoader.h"

#include "../stubs/StubDataNode.h"
#include "../stubs/StubDataParser.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <vector>

class ComboLoaderFixture
{
public:
    std::unique_ptr<StubDataNode> makeStep(const std::string& action, float maxDelay) const
    {
        auto step = std::make_unique<StubDataNode>();
        step->setString("action", action);
        step->setFloat("maxDelay", maxDelay);
        return step;
    }

    std::unique_ptr<StubDataNode> makeCombo(
        const std::string& name,
        const std::string& trigger,
        int priority,
        bool consume,
        std::vector<std::unique_ptr<StubDataNode>> steps
    ) const {
        auto combo = std::make_unique<StubDataNode>();
        combo->setString("name", name);
        combo->setString("trigger", trigger);
        combo->setInt("priority", priority);
        combo->setBool("consume", consume);

        std::vector<std::unique_ptr<DataNode>> stepsData;
        for (auto& s : steps) stepsData.push_back(std::move(s));
        combo->setArray("steps", std::move(stepsData));
        return combo;
    }

    std::unique_ptr<StubDataNode> makeCombosRoot() const
    {
        auto root = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> combos;
        combos.push_back(makeCombo("uppercut", "Punched", 10, false, {
            this->makeStep("Punch", 150.f),
            this->makeStep("Jump", 100.f)
        }));
        combos.push_back(makeCombo("guard", "Kicked", 5, true, {this->makeStep("Defend", 80.f)}));
        root->setArray("combos", std::move(combos));
        return root;
    }
};

TEST_CASE_METHOD(ComboLoaderFixture, "ComboLoader parses combos with optional fields",
    "[unit][combo_loader]"
) {
    StubDataParser parser;
    parser.registerNode("combos.json", this->makeCombosRoot());

    ComboLoader loader(parser);
    const auto combos = loader.load("combos.json");

    REQUIRE(combos.size() == 2);
    REQUIRE(combos[0].name == "uppercut");
    REQUIRE(combos[0].trigger == TriggerId::Punched);
    REQUIRE(combos[0].priority == 10);
    REQUIRE(combos[0].consumeInput == false);
    REQUIRE(combos[0].steps.size() == 2);
    REQUIRE(combos[0].steps[0].action == InputAction::Punch);
    REQUIRE(combos[0].steps[0].maxDelay == 150.f);
    REQUIRE(combos[0].steps[1].action == InputAction::Jump);
    REQUIRE(combos[0].steps[1].maxDelay == 100.f);

    REQUIRE(combos[1].name == "guard");
    REQUIRE(combos[1].trigger == TriggerId::Kicked);
    REQUIRE(combos[1].priority == 5);
    REQUIRE(combos[1].consumeInput == true);
    REQUIRE(combos[1].steps.size() == 1);
    REQUIRE(combos[1].steps[0].action == InputAction::Defend);
    REQUIRE(combos[1].steps[0].maxDelay == 80.f);
}

TEST_CASE_METHOD(ComboLoaderFixture, "ComboLoader forwards file path to parser",
    "[unit][combo_loader]"
) {
    StubDataParser parser;
    parser.registerNode("custom/combos.json", this->makeCombosRoot());

    ComboLoader loader(parser);
    REQUIRE_NOTHROW(loader.load("custom/combos.json"));
}
