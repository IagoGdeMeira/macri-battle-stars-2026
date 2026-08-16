#ifndef align_items_handler_h
#define align_items_handler_h

#include "IFlexLayoutHandler/IFlexLayoutHandler.h"

class AlignItemsHandler : public IFlexLayoutHandler
{
public:
    void layout(FlexLayoutContext& ctx) override;
};

#endif // align_items_handler_h
