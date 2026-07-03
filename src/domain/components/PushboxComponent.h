#ifndef pushbox_component_h
#define pushbox_component_h

struct PushboxComponent
{
    enum class PushboxType { Static, Dynamic };

    PushboxType type = PushboxType::Dynamic;
    float mass = 1.f, pushResistance = 1.f;
};

#endif // pushbox_component_h
