#ifndef system_manager_h
#define system_manager_h

#include "../System/System.h"
#include "../UpdateContext/UpdateContext.h"

#include <memory>
#include <vector>

class SystemManager
{
public:
    ~SystemManager() = default;

    template <typename T, typename... Args>
    T& addSystem(Args&&... args);

    void update(UpdateContext& ctx) { for (const auto& system : this->systems) system->update(ctx); }

private:
    std::vector<std::unique_ptr<System>> systems;
};

#include "SystemManager.inl"

#endif // system_manager_h
