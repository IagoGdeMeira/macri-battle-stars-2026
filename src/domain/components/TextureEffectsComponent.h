#ifndef texture_effects_component_h
#define texture_effects_component_h

#include <functional>
#include <vector>

struct TextureEffectsComponent
{
    using Effect = std::function<void(void*, void*)>;
    std::vector<Effect> effects;
};

#endif // texture_effects_component_h
