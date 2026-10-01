#ifndef world_h
#define world_h

#include "ComponentManager/ComponentManager.h"
#include "EntityManager/EntityManager.h"
#include "View/View.h"

class World
{
public:
    World() = default;

    EntityManager& entities() { return this->entityManager; }
    const EntityManager& entities() const { return this->entityManager; }

    ComponentManager& components() { return this->componentManager; }
    const ComponentManager& components() const { return this->componentManager; }

    void destroy(Entity entity)
    {
        if (!this->entityManager.isAlive(entity)) return;
        this->componentManager.entityDestroyed(entity);
        this->entityManager.destroy(entity);
    }

    void clear() { this->entityManager.clear(); this->componentManager.clear(); }    

private:
    EntityManager entityManager;
    ComponentManager componentManager;
};

#endif // world_h
