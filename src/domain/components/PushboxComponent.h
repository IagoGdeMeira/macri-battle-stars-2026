#ifndef pushbox_component_h
#define pushbox_component_h

struct PushboxComponent
{
    enum class PushboxType { Static, Dynamic };

    PushboxType type = PushboxType::Dynamic;
    float mass = 1.0f, pushResistance = 1.0f;
};

#endif // pushbox_component_h
