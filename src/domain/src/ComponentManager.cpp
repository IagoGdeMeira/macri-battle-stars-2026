#include "../include/ComponentManager/ComponentManager.h"

inline void ComponentManager::removeAll(Entity e)
{ for (auto& storage : this->storages) storage->remove(e); }
