#ifndef entity_h
#define entity_h

#include <cstddef>
#include <cstdint>

class Entity
{
public:
    Entity(size_t id, size_t generation) : id(id), generation(generation) {}

    size_t index() const noexcept { return this->id; }
    size_t gen() const noexcept { return this->generation; }

    bool operator==(const Entity &other) const
    { return this->id == other.id && this->generation == other.generation; }

    bool operator!=(const Entity &other) const { return !(*this == other); }

private:
    size_t id{0};
    size_t generation{0};
};

#endif // entity_h
