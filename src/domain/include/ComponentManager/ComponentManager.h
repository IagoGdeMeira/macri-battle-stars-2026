#ifndef component_manager_h
#define component_manager_h

#include "../ComponentStorage/ComponentStorage.h"
#include "../Entity/Entity.h"

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
    void add(Entity entity, const T& component) { this->getStorage<T>()->add(entity, component); }
    template <typename T>
    void add(Entity entity, T&& component) { this->getStorage<T>()->add(entity, std::move(component)); }

    template <typename T>
    void remove(Entity entity) { this->getStorage<T>()->remove(entity); }

    template <typename T>
    bool has(Entity entity) const { return this->getStorage<T>()->has(entity); }

    template <typename T>
    T& get(Entity entity) { return this->getStorage<T>()->get(entity); }
    template <typename T>
    const T& get(Entity entity) const { return this->getStorage<T>()->get(entity); }

    template <typename T>
    ComponentStorage<T>* storage() { return this->getStorage<T>(); }
    template <typename T>
    const ComponentStorage<T>* storage() const { return this->getStorage<T>(); }

    void entityDestroyed(Entity entity);

private:
    std::vector<std::unique_ptr<IComponentStorage>> storages;

    static uint32_t nextComponentTypeId;

    template <typename T>
    static uint32_t getTypeId()
    {
        static uint32_t id = ComponentManager::nextComponentTypeId++;
        return id;
    }

    template <typename T>
    ComponentStorage<T>* getStorage();
    template <typename T>
    const ComponentStorage<T>* getStorage() const;
};

#include "ComponentManager.inl"

#endif // component_manager_h
