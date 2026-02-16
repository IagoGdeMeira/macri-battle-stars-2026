#include "../include/World/World.h"

Entity World::createEntity() { return this->entities.create(); }

void World::destroyEntity(Entity e)
{
    this->components.removeAll(e);
    this->signatures.clear(e);
    this->entities.destroy(e);
}
