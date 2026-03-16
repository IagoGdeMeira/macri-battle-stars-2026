#ifndef world_h
#define world_h

#include "../ComponentManager/ComponentManager.h"
#include "../EntityManager/EntityManager.h"
#include "../View/View.h"

class World
{
public:
    World() = default;

    Entity create();
    void destroy(Entity entity);

    template <typename T>
    void registerComponent();

    template <typename T>
    void add(Entity entity, const T& component);

    template <typename T>
    void remove(Entity entity);

    template <typename T>
    bool has(Entity entity) const;

    template <typename T>
    T& get(Entity entity);

    template <typename... Components>
    View<Components...> view();

private:
    EntityManager entityManager;
    ComponentManager componentManager;
};

#include "World.inl"

#endif // world_h
