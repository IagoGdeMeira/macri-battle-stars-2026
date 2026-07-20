#ifndef pushbox_component_h
#define pushbox_component_h

struct PushboxComponent
{
    enum class Type { Static, Dynamic };

    Type type = Type::Dynamic;
    float mass = 1.f, pushResistance = 1.f;
};

#endif // pushbox_component_h
