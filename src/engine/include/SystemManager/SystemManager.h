#ifndef system_manager_h
#define system_manager_h

#include "../System/System.h"

#include <memory>
#include <vector>

struct UpdateContext;

class SystemManager
{
public:
    ~SystemManager();

    template <typename T, typename... Args>
    T& addSystem(Args&&... args);

    void update(UpdateContext& context);

private:
    std::vector<std::unique_ptr<System>> systems;
};

#include "SystemManager.inl"

#endif // system_manager_h
