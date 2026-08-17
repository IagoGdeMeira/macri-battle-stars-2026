#include "FlexEnumMapper/FlexEnumMapper.h"

#include <stdexcept>

FlexEnumMapper::FlexDir FlexEnumMapper::toDirection(const std::string& value)
{
    if (value == "Row")             return FlexDir::Row;
    if (value == "Column")          return FlexDir::Column;
    if (value == "RowReverse")      return FlexDir::RowReverse;
    if (value == "ColumnReverse")   return FlexDir::ColumnReverse;

    throw std::runtime_error("Invalid FlexDirection: " + value);
}

FlexEnumMapper::Justify FlexEnumMapper::toJustify(const std::string& value)
{
    if (value == "FlexStart")       return Justify::FlexStart;
    if (value == "FlexEnd")         return Justify::FlexEnd;
    if (value == "Center")          return Justify::Center;
    if (value == "SpaceBetween")    return Justify::SpaceBetween;
    if (value == "SpaceAround")     return Justify::SpaceAround;
    if (value == "SpaceEvenly")     return Justify::SpaceEvenly;

    throw std::runtime_error("Invalid JustifyContent: " + value);
}

FlexEnumMapper::Align FlexEnumMapper::toAlign(const std::string& value)
{
    if (value == "FlexStart")   return Align::FlexStart;
    if (value == "FlexEnd")     return Align::FlexEnd;
    if (value == "Center")      return Align::Center;
    if (value == "Stretch")     return Align::Stretch;
    if (value == "Baseline")    return Align::Baseline;

    throw std::runtime_error("Invalid AlignItems: " + value);
}
