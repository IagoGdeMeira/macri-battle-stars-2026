#ifndef entity_h
#define entity_h

#include <cstdint>

struct Entity
{
    Entity(uint32_t id) : id(id) {}

    uint32_t id;

    bool operator==(const Entity &other) const { return this->id == other.id; }
    bool operator!=(const Entity &other) const { return this->id != other.id; }
};

#endif // entity_h
