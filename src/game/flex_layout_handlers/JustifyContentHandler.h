#ifndef justify_content_handler_h
#define justify_content_handler_h

#include "IFlexLayoutHandler/IFlexLayoutHandler.h"

class JustifyContentHandler : public IFlexLayoutHandler
{
public:
    void layout(FlexLayoutContext& ctx) override;
};

#endif // justify_content_handler_h
