#ifndef i_visual_effect_h
#define i_visual_effect_h

#include "engine/include/RenderQueue/RenderQueue.h"

template <typename CommandType>
class IVisualEffect
{
public:
    virtual ~IVisualEffect() = default;
    virtual void apply(RenderQueue& queue, const CommandType& base) const = 0;
};

#endif // i_visual_effect_h
