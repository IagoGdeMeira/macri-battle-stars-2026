#ifndef world_h
#define world_h

#include "../ComponentManager/ComponentManager.h"
#include "../EntityManager/EntityManager.h"
#include "../View/View.h"

class World
{
public:
    World() = default;

    EntityManager& entities() { return this->entityManager; }
    ComponentManager& components() { return this->componentManager; }

private:
    EntityManager entityManager;
    ComponentManager componentManager;
};

#endif // world_h
