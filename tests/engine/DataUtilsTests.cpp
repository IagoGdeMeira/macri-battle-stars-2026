#include "../../src/engine/include/DataUtils/DataUtils.h"

#include "../stubs/StubDataNode.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("DataUtils::parsePosition reads x and y",
    "[unit][data_utils]")
{
    StubDataNode node;
    node.setFloat("x", 1.5f);
    node.setFloat("y", 2.25f);

    Position result = DataUtils::parsePosition(node, {0.f, 0.f});

    REQUIRE(result.x == 1.5f);
    REQUIRE(result.y == 2.25f);
}

TEST_CASE("DataUtils::parsePosition returns default when missing",
    "[unit][data_utils]")
{
    StubDataNode node;
    node.setFloat("x", 1.5f);

    Position def{5.f, 6.f};

    Position result = DataUtils::parsePosition(node, def);

    REQUIRE(result.x == def.x);
    REQUIRE(result.y == def.y);
}

TEST_CASE("DataUtils::parseSize reads w and h",
    "[unit][data_utils]")
{
    StubDataNode node;
    node.setFloat("w", 10.f);
    node.setFloat("h", 20.f);

    Dimension2D result = DataUtils::parseSize(node, {0.f, 0.f});

    REQUIRE(result.width == 10.f);
    REQUIRE(result.height == 20.f);
}

TEST_CASE("DataUtils::parseRect reads nested position and size",
    "[unit][data_utils]")
{
    StubDataNode node;

    auto pos = std::make_unique<StubDataNode>();
    pos->setFloat("x", 3.f);
    pos->setFloat("y", 4.f);
    node.setObject("position", std::move(pos));

    auto size = std::make_unique<StubDataNode>();
    size->setFloat("w", 7.f);
    size->setFloat("h", 8.f);
    node.setObject("size", std::move(size));

    Rectangle def{{0.f, 0.f}, {0.f, 0.f}};

    Rectangle result = DataUtils::parseRect(node, def);

    REQUIRE(result.position.x == 3.f);
    REQUIRE(result.position.y == 4.f);
    REQUIRE(result.size.width == 7.f);
    REQUIRE(result.size.height == 8.f);
}

TEST_CASE("DataUtils::parseColor reads r,g,b,a",
    "[unit][data_utils]")
{
    StubDataNode node;
    node.setInt("r", 10);
    node.setInt("g", 20);
    node.setInt("b", 30);
    node.setInt("a", 128);

    Color def{255, 255, 255, 255};

    Color result = DataUtils::parseColor(node, def);

    REQUIRE(result.r == 10);
    REQUIRE(result.g == 20);
    REQUIRE(result.b == 30);
    REQUIRE(result.a == 128);
}

TEST_CASE("DataUtils::parseAABB reads left,right,top,bottom",
    "[unit][data_utils]")
{
    StubDataNode node;
    node.setFloat("left", 1.f);
    node.setFloat("right", 2.f);
    node.setFloat("top", 3.f);
    node.setFloat("bottom", 4.f);

    AABB result = DataUtils::parseAABB(node, {0.f, 0.f, 0.f, 0.f});

    REQUIRE(result.left == 1.f);
    REQUIRE(result.right == 2.f);
    REQUIRE(result.top == 3.f);
    REQUIRE(result.bottom == 4.f);
}

TEST_CASE("DataUtils::parseCorners reads topLeft, topRight, bottomLeft, bottomRight", "[unit][data_utils]")
{
    StubDataNode node;
    node.setFloat("topLeft", 1.f);
    node.setFloat("topRight", 3.f);
    node.setFloat("bottomLeft", 5.f);
    node.setFloat("bottomRight", 7.f);

    Corners result = DataUtils::parseCorners(node, {0.f, 0.f, 0.f, 0.f});

    REQUIRE(result.topLeft == 1.f);
    REQUIRE(result.topRight == 3.f);
    REQUIRE(result.bottomLeft == 5.f);
    REQUIRE(result.bottomRight == 7.f);
}
