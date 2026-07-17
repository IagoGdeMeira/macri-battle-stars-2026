#ifndef component_storage_h
#define component_storage_h

#include "Entity/Entity.h"

#include <vector>

class IComponentStorage
{
public:
    virtual ~IComponentStorage() = default;

    virtual void remove(Entity e) = 0;
    virtual bool has(Entity e) const = 0;
    virtual size_t size() const = 0;

    virtual const std::vector<Entity>& entities() const = 0;
};

template <typename T>
class ComponentStorage : public IComponentStorage
{
public:
    template <typename U>
    void add(Entity entity, U&& component);

    void remove(Entity entity) override;
    bool has(Entity entity) const override;
    size_t size() const override { return this->denseComponents.size(); }

    const std::vector<Entity>& entities() const override { return this->denseEntities; }

    T& get(Entity entity);
    const T& get(Entity entity) const { return const_cast<ComponentStorage<T>*>(this)->get(entity); }

private:
    std::vector<Entity> denseEntities;
    std::vector<T> denseComponents;
    std::vector<uint32_t> sparse;
};

#include "ComponentStorage.inl"

#endif // component_storage_h
