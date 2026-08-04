#ifndef circle_effects_component_h
#define circle_effects_component_h

#include <functional>
#include <vector>

struct CircleEffectsComponent
{
    using Effect = std::function<void(void*, void*)>;
    std::vector<Effect> effects;
};

#endif // circle_effects_component_h
