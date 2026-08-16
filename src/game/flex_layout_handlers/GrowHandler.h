#ifndef grow_handler_h
#define grow_handler_h

#include "IFlexLayoutHandler/IFlexLayoutHandler.h"

class GrowHandler : public IFlexLayoutHandler
{
public:
    void layout(FlexLayoutContext& ctx) override;
};

#endif // grow_handler_h
