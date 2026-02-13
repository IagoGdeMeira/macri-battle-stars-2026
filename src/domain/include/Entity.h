#ifndef entity_h
#define entity_h

#include <cstdint>

struct Entity
{
    uint32_t id{0};

    bool operator==(const Entity &other) const { return id == other.id; }
    bool operator!=(const Entity &other) const { return id != other.id; }
};

#endif // entity_h
