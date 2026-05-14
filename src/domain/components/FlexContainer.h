#ifndef flex_container_h
#define flex_container_h

struct FlexContainer
{
    enum class FlexDirection { Row, Column, RowReverse, ColumnReverse };
    enum class JustifyContent { FlexStart, FlexEnd, Center, SpaceBetween, SpaceAround, SpaceEvenly };
    enum class AlignItems { FlexStart, FlexEnd, Center, Stretch, Baseline };

    FlexDirection direction = FlexDirection::Row;
    JustifyContent justify = JustifyContent::FlexStart;
    AlignItems align = AlignItems::Stretch;

    float gap = 0.0f;
    bool needsLayout = true;
};

#endif // flex_container_h
