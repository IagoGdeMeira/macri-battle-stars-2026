#include "../include/SystemManager/SystemManager.h"

#include "../include/System/System.h"
#include "../include/UpdateContext/UpdateContext.h"

SystemManager::~SystemManager() = default;

void SystemManager::update(UpdateContext& context)
{ for (const auto& system : this->systems) system->update(context); }
