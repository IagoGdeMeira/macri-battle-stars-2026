#ifndef system_manager_h
#define system_manager_h

#include <memory>
#include <vector>

class System;

struct UpdateContext;

class SystemManager
{
public:
    template <typename T, typename... Args>
    T& addSystem(Args&&... args);

    void update(UpdateContext& context);

private:
    std::vector<std::unique_ptr<System>> systems;
};

#include "SystemManager.inl"

#endif // system_manager_h