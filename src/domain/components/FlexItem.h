#ifndef flex_item_h
#define flex_item_h

struct FlexItem
{
    float grow = .0f, shrink = 1.0f, basis = -1.0f;
    bool isFocused = false;
};

#endif // flex_item_h
