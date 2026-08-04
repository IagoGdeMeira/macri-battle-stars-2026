#ifndef font_effects_component_h
#define font_effects_component_h

#include <functional>
#include <vector>

struct FontEffectsComponent
{
    using Effect = std::function<void(void*, void*)>;
    std::vector<Effect> effects;
};

#endif // font_effects_component_h
