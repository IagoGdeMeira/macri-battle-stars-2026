#ifndef main_axis_handler_h
#define main_axis_handler_h

#include "IFlexLayoutHandler/IFlexLayoutHandler.h"

class MainAxisHandler : public IFlexLayoutHandler
{
public:
    void layout(FlexLayoutContext& ctx) override;
};

#endif // main_axis_handler_h
