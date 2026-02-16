#include "../include/ComponentManager/ComponentManager.h"

void ComponentManager::removeAll(Entity e)
{ for (auto& storage : this->storages) if(storage) storage->remove(e); }
