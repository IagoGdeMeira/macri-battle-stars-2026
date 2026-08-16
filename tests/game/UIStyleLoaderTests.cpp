#include "game/include/UIStyleLoader/UIStyleLoader.h"

#include "StubDataNode.h"
#include "StubDataParser.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <vector>

class UIStyleLoaderFixture
{
public:
    StubDataParser parser;
    StubDataNode makeStyleNode(const std::string& id, float paddingLeft = 0.f, float paddingRight = 0.f)
    {
        StubDataNode node;
        node.setString("id", id);
        auto padding = std::make_unique<StubDataNode>();
        padding->setFloat("left", paddingLeft);
        padding->setFloat("right", paddingRight);
        padding->setFloat("top", 0.f);
        padding->setFloat("bottom", 0.f);
        node.setObject("padding", std::move(padding));
        return node;
    }
};

TEST_CASE_METHOD(UIStyleLoaderFixture, "UIStyleLoader loads styles from a stylesheet", "[unit][ui_style_loader]")
{
    StubDataNode root;
    std::vector<std::unique_ptr<DataNode>> stylesArray;
    stylesArray.push_back(std::make_unique<StubDataNode>(makeStyleNode("primaryButton", 8.f, 8.f)));
    stylesArray.push_back(std::make_unique<StubDataNode>(makeStyleNode("healthBar", 2.f, 2.f)));
    root.setArray("styles", std::move(stylesArray));
    this->parser.registerNode("styles.json", std::make_unique<StubDataNode>(root));

    UIStyleLoader loader(this->parser);
    auto styles = loader.load("styles.json");

    REQUIRE(styles.size() == 2);
    REQUIRE(styles.contains("primaryButton"));
    REQUIRE(styles.contains("healthBar"));

    auto& primary = styles.at("primaryButton");
    REQUIRE(primary->getString("id") == "primaryButton");
    auto paddingNode = primary->getObject("padding");
    REQUIRE(paddingNode != nullptr);
    REQUIRE(paddingNode->getFloat("left") == 8.f);
    REQUIRE(paddingNode->getFloat("right") == 8.f);
}

TEST_CASE_METHOD(UIStyleLoaderFixture, "UIStyleLoader returns empty map when no styles", "[unit][ui_style_loader]")
{
    StubDataNode root;
    root.setArray("styles", {});
    this->parser.registerNode("empty_styles.json", std::make_unique<StubDataNode>(root));

    UIStyleLoader loader(this->parser);
    auto styles = loader.load("empty_styles.json");

    REQUIRE(styles.empty());
}
