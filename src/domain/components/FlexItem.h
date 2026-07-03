#ifndef flex_item_h
#define flex_item_h

struct FlexItem
{
    float grow = 0.f, shrink = 1.f, basis = -1.f;
    bool isFocused = false;
};

#endif // flex_item_h
