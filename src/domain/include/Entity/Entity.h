#ifndef entity_h
#define entity_h

#include <cstdint>

struct Entity
{
    uint32_t id{0};
    uint32_t generation{0};

    bool operator==(const Entity &other) const
    { return this->id == other.id && this->generation == other.generation; }

    bool operator!=(const Entity &other) const { return !(*this == other); }
};

#endif // entity_h
