#include "game/include/FlexEnumMapper/FlexEnumMapper.h"

#include "domain/value_objects/FlexEnums/FlexEnums.h"

#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

TEST_CASE("FlexEnumMapper maps direction strings", "[unit][flex_enum_mapper]")
{
    using FlexDir = FlexEnumMapper::FlexDir;
    REQUIRE(FlexEnumMapper::toDirection("Row") == FlexDir::Row);
    REQUIRE(FlexEnumMapper::toDirection("Column") == FlexDir::Column);
    REQUIRE(FlexEnumMapper::toDirection("RowReverse") == FlexDir::RowReverse);
    REQUIRE(FlexEnumMapper::toDirection("ColumnReverse") == FlexDir::ColumnReverse);
    REQUIRE_THROWS_AS(FlexEnumMapper::toDirection("Invalid"), std::runtime_error);
}

TEST_CASE("FlexEnumMapper maps justify strings", "[unit][flex_enum_mapper]")
{
    using Justify = FlexEnumMapper::Justify;
    REQUIRE(FlexEnumMapper::toJustify("FlexStart") == Justify::FlexStart);
    REQUIRE(FlexEnumMapper::toJustify("FlexEnd") == Justify::FlexEnd);
    REQUIRE(FlexEnumMapper::toJustify("Center") == Justify::Center);
    REQUIRE(FlexEnumMapper::toJustify("SpaceBetween") == Justify::SpaceBetween);
    REQUIRE(FlexEnumMapper::toJustify("SpaceAround") == Justify::SpaceAround);
    REQUIRE(FlexEnumMapper::toJustify("SpaceEvenly") == Justify::SpaceEvenly);
    REQUIRE_THROWS_AS(FlexEnumMapper::toJustify("Invalid"), std::runtime_error);
}

TEST_CASE("FlexEnumMapper maps align strings", "[unit][flex_enum_mapper]")
{
    using Align = FlexEnumMapper::Align;
    REQUIRE(FlexEnumMapper::toAlign("FlexStart") == Align::FlexStart);
    REQUIRE(FlexEnumMapper::toAlign("FlexEnd") == Align::FlexEnd);
    REQUIRE(FlexEnumMapper::toAlign("Center") == Align::Center);
    REQUIRE(FlexEnumMapper::toAlign("Stretch") == Align::Stretch);
    REQUIRE(FlexEnumMapper::toAlign("Baseline") == Align::Baseline);
    REQUIRE_THROWS_AS(FlexEnumMapper::toAlign("Invalid"), std::runtime_error);
}
