#ifndef component_storage_h
#define component_storage_h

#include "../Entity/Entity.h"
#include "../IComponentStorage/IComponentStorage.h"

#include <vector>

template <typename T>
class ComponentStorage : public IComponentStorage
{
public:
    void add(Entity entity, const T& component);

    void remove(Entity entity) override;

    bool has(Entity entity) const override;

    T& get(Entity entity);

    const T& get(Entity entity) const;

    size_t size() const override;

    const std::vector<Entity>& entities() const override;

private:
    std::vector<Entity> denseEntities;

    std::vector<T> denseComponents;

    std::vector<uint32_t> sparse;
};

#include "ComponentStorage.inl"

#endif // component_storage_h
