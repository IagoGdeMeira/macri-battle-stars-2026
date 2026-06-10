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

TEST_CASE("DataUtils::parseCorners returns default when missing", "[unit][data_utils]")
{
    StubDataNode node;
    node.setFloat("topLeft", 1.f);
    node.setFloat("topRight", 3.f);

    Corners def{10.f, 20.f, 30.f, 40.f};

    Corners result = DataUtils::parseCorners(node, def);

    REQUIRE(result.topLeft == def.topLeft);
    REQUIRE(result.topRight == def.topRight);
    REQUIRE(result.bottomLeft == def.bottomLeft);
    REQUIRE(result.bottomRight == def.bottomRight);
}

TEST_CASE("DataUtils::setPosition sets x and y", "[unit][data_utils]")
{
    StubDataNode node;

    Position pos{1.5f, 2.25f};
    DataUtils::setPosition(node, pos);

    REQUIRE(node.getFloat("x") == 1.5f);
    REQUIRE(node.getFloat("y") == 2.25f);
}

TEST_CASE("DataUtils::setSize sets w and h", "[unit][data_utils]")
{
    StubDataNode node;

    Dimension2D size{10.f, 20.f};
    DataUtils::setSize(node, size);

    REQUIRE(node.getFloat("w") == 10.f);
    REQUIRE(node.getFloat("h") == 20.f);
}

TEST_CASE("DataUtils::setRect sets nested position and size", "[unit][data_utils]")
{
    StubDataNode node;

    Rectangle rect{{3.f, 4.f}, {7.f, 8.f}};
    DataUtils::setRect(node, rect);

    auto posNode = node.getObject("position");
    REQUIRE(posNode != nullptr);
    REQUIRE(posNode->getFloat("x") == 3.f);
    REQUIRE(posNode->getFloat("y") == 4.f);

    auto sizeNode = node.getObject("size");
    REQUIRE(sizeNode != nullptr);
    REQUIRE(sizeNode->getFloat("w") == 7.f);
    REQUIRE(sizeNode->getFloat("h") == 8.f);
}

TEST_CASE("DataUtils::setColor sets r,g,b,a", "[unit][data_utils]")
{
    StubDataNode node;

    Color color{10, 20, 30, 128};
    DataUtils::setColor(node, color);

    REQUIRE(node.getInt("r") == 10);
    REQUIRE(node.getInt("g") == 20);
    REQUIRE(node.getInt("b") == 30);
    REQUIRE(node.getInt("a") == 128);
}

TEST_CASE("DataUtils::setAABB sets left,right,top,bottom", "[unit][data_utils]")
{
    StubDataNode node;

    AABB aabb{1.f, 2.f, 3.f, 4.f};
    DataUtils::setAABB(node, aabb);

    REQUIRE(node.getFloat("left") == 1.f);
    REQUIRE(node.getFloat("right") == 2.f);
    REQUIRE(node.getFloat("top") == 3.f);
    REQUIRE(node.getFloat("bottom") == 4.f);
}

TEST_CASE("DataUtils::setCorners sets topLeft, topRight, bottomLeft, bottomRight", "[unit][data_utils]")
{
    StubDataNode node;

    Corners corners{1.f, 3.f, 5.f, 7.f};
    DataUtils::setCorners(node, corners);

    REQUIRE(node.getFloat("topLeft") == 1.f);
    REQUIRE(node.getFloat("topRight") == 3.f);
    REQUIRE(node.getFloat("bottomLeft") == 5.f);
    REQUIRE(node.getFloat("bottomRight") == 7.f);
}
