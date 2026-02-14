#include "../include/ComponentManager/ComponentManager.h"

inline void ComponentManager::removeAll(Entity e)
{ for (auto& [_, storage] : this->storages) storage->remove(e); }
