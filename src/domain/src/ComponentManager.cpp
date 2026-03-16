#include "../include/ComponentManager/ComponentManager.h"

uint32_t ComponentManager::nextComponentTypeId = 0;

void ComponentManager::entityDestroyed(Entity entity)
{ for (auto& storage : this->storages) if (storage) storage->remove(entity); }
