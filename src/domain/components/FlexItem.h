#ifndef flex_item_h
#define flex_item_h

#include "domain/value_objects/FlexEnums/FlexEnums.h"

#include <optional>

struct FlexItem
{
    float grow = 0.f, shrink = 1.f, basis = -1.f;
    std::optional<AlignItems> alignSelf = std::nullopt;
    bool isFocused = false;
};

#endif // flex_item_h
