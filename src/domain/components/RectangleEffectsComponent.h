#ifndef rectangle_effects_component_h
#define rectangle_effects_component_h

#include <functional>
#include <vector>

struct RectangleEffectsComponent
{
    using Effect = std::function<void(void*, void*)>;
    std::vector<Effect> effects;
};

#endif // rectangle_effects_component_h
