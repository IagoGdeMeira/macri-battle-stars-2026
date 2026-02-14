#ifndef world_h
#define world_h

#include "ComponentStorage.h"
#include "Entity.h"

#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>

class World
{
public:
    Entity createEntity();

    void destroyEntity(Entity entity);

    template <typename ComponentType>
    void addComponent(Entity entity, const ComponentType &component);

    template <typename ComponentType>
    void removeComponent(Entity entity);

    template <typename ComponentType>
    bool hasComponent(Entity entity) const;

    template <typename ComponentType>
    ComponentType &getComponent(Entity entity);

    template<typename... Components, typename Func>
    void each(Func func);

private:
    std::uint32_t nextEntityId = 1;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> storages;

    template <typename ComponentType>
    ComponentStorage<ComponentType> *findStorage();

    template <typename ComponentType>
    const ComponentStorage<ComponentType> *findStorage() const;

    template <typename ComponentType>
    ComponentStorage<ComponentType> *getOrCreateStorage();
};

#include "World.inl"

#endif // world_h
