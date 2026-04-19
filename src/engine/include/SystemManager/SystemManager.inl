#ifndef system_manager_inl
#define system_manager_inl

#include "SystemManager.h"

#include <utility>

template <typename T, typename... Args>
T& SystemManager::addSystem(Args&&... args)
{
    auto system = std::make_unique<T>(std::forward<Args>(args)...);

    T& ref = *system;
    
    this->systems.push_back(std::move(system));
    
    return ref;
}

#endif // system_manager_inl
