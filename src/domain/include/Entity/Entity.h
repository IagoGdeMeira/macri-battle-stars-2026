#ifndef entity_h
#define entity_h

#include <cstdint>
#include <functional>

struct Entity
{
    Entity(uint32_t id) : id(id) {}

    uint32_t id;

    bool operator==(const Entity& other) const { return this->id == other.id; }
    bool operator!=(const Entity& other) const { return this->id != other.id; }

    struct Hash
    {
        size_t operator()(const Entity& entity) const noexcept
        { return std::hash<uint32_t>{}(entity.id); }
    };
};

#endif // entity_h
