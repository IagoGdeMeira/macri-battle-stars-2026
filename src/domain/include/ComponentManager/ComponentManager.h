#ifndef component_manager_h
#define component_manager_h

#include "../ComponentStorage/ComponentStorage.h"
#include "../Entity/Entity.h"
#include "../IComponentStorage/IComponentStorage.h"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

class ComponentManager
{
public:
    template <typename T>
    void registerComponent();

    template <typename T>
    void add(Entity entity, const T& component);

    template <typename T>
    void add(Entity entity, T&& component);

    template <typename T>
    void remove(Entity entity);

    template <typename T>
    bool has(Entity entity) const;

    template <typename T>
    T& get(Entity entity);

    template <typename T>
    IComponentStorage* storage();

    template <typename T>
    const ComponentStorage<T>* storage() const;

    void entityDestroyed(Entity entity);

private:
    std::vector<std::unique_ptr<IComponentStorage>> storages;

    static uint32_t nextComponentTypeId;

    template <typename T>
    static uint32_t componentTypeId();

    template <typename T>
    ComponentStorage<T>* getStorage();

    template <typename T>
    const ComponentStorage<T>* getStorage() const;
};

#include "ComponentManager.inl"

#endif // component_manager_h
