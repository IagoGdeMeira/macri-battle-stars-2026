#ifndef box_model_handler_h
#define box_model_handler_h

#include "IFlexLayoutHandler/IFlexLayoutHandler.h"

class BoxModelHandler : public IFlexLayoutHandler
{
public:
    void layout(FlexLayoutContext& ctx) override;
};

#endif // box_model_handler_h
