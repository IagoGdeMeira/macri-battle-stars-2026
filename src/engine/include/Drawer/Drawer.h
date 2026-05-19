#ifndef drawer_h
#define drawer_h

#include "../RenderContext/RenderContext.h"

class Drawer
{
public:
    virtual ~Drawer() = default;
    virtual void draw(RenderContext& ctx) = 0;
};

#endif // drawer_h
