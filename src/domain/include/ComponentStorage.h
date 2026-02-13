#ifndef component_storage_h
#define component_storage_h

#include "Entity.h"
#include "IComponentStorage.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

template <typename ComponentType>
class ComponentStorage : public IComponentStorage
{
public:
    void add(Entity e, const ComponentType &component);

    void remove(Entity e) override;

    bool has(Entity e) const;

    ComponentType &get(Entity e);

    const std::vector<ComponentType> &data() const { return this->components; }
    const std::vector<Entity> &owners() const { return this->entities; }

private:
    std::vector<ComponentType> components;
    std::vector<Entity> entities;
    std::unordered_map<std::uint32_t, size_t> entityToIndex;
};

#include "ComponentStorage.inl"

#endif // component_storage_h
