#ifndef flex_enum_mapper_h
#define flex_enum_mapper_h

#include "domain/components/FlexContainer.h"
#include "domain/value_objects/FlexEnums/FlexEnums.h"

#include <string>

class FlexEnumMapper
{
public:
    using FlexDir = FlexDirection;
    using Justify = JustifyContent;
    using Align = AlignItems;

    static FlexDir toDirection(const std::string& value);
    static Justify toJustify(const std::string& value);
    static Align toAlign(const std::string& value);
};

#endif // flex_enum_mapper_h
