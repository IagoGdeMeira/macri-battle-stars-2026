#include "../include/World.h"

Entity World::createEntity() { return Entity{ this->nextEntityId++ }; }

void World::destroyEntity(Entity entity)
{ for (auto &[_, storage] : this->storages) if (storage) storage->remove(entity); }
