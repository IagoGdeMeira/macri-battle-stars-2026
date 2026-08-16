#ifndef i_flex_layout_handler_h
#define i_flex_layout_handler_h

struct FlexLayoutContext;

class IFlexLayoutHandler
{
public:
    virtual ~IFlexLayoutHandler() = default;
    virtual void layout(FlexLayoutContext& ctx) = 0;
};

#endif // i_flex_layout_handler_h
