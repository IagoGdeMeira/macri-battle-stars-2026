#ifndef world_h
#define world_h

#include "../ComponentManager/ComponentManager.h"
#include "../EntityManager/EntityManager.h"
#include "../SignatureManager/SignatureManager.h"

class World
{
public:
    Entity createEntity();
    void destroyEntity(Entity e);

    template <typename Component>
    void addComponent(Entity e, const Component &component);

    template <typename Component>
    void removeComponent(Entity e);

    template <typename Component>
    Component &getComponent(Entity e);

    template <typename... Components, typename Func>
    void each(Func func);

private:
    EntityManager entities;
    ComponentManager components;
    SignatureManager signatures;
};

#include "World.inl"

#endif // world_h
