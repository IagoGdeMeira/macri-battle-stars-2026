#ifndef component_storage_h
#define component_storage_h

#include "../Entity/Entity.h"
#include "../IComponentStorage/IComponentStorage.h"

#include <vector>

template <typename ComponentType>
class ComponentStorage : public IComponentStorage
{
public:
    void add(Entity e, const ComponentType &component);
    void remove(Entity e) override;

    bool has(Entity e) const;
    ComponentType &get(Entity e);

    size_t size() const;

    std::vector<ComponentType> &getComponents();
    const std::vector<ComponentType> &getComponents() const;

    const std::vector<Entity> &getEntities() const;

private:
    std::vector<ComponentType> components;
    std::vector<Entity> entities;
    std::vector<size_t> sparse;
};

#include "ComponentStorage.inl"

#endif // component_storage_h
