#ifndef component_manager_h
#define component_manager_h

#include "../ComponentStorage/ComponentStorage.h"
#include "../Entity/Entity.h"

#include <memory>
#include <typeindex>
#include <unordered_map>

class ComponentManager
{
public:
    template <typename Component>
    void add(Entity e, const Component &component);

    template <typename Component>
    void remove(Entity e);

    template <typename Component>
    Component &get(Entity e);

    template <typename Component>
    bool has(Entity e) const;

    template <typename Component>
    ComponentStorage<Component> *findStorage();

    template <typename Component>
    const ComponentStorage<Component> *findStorage() const;

    void removeAll(Entity e);

private:
    std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> storages;

    template <typename Component>
    ComponentStorage<Component> *getOrCreateStorage();
};

#include "ComponentManager.inl"

#endif // component_manager_h
