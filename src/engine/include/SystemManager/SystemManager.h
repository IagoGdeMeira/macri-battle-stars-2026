#ifndef system_manager_h
#define system_manager_h

#include "../System/System.h"
#include "../UpdateContext/UpdateContext.h"

#include "../../domain/utils/Logger/Logger.h"

#include <memory>
#include <vector>

class SystemManager
{
public:
    ~SystemManager() = default;

    template <typename T, typename... Args>
    T& addSystem(Args&&... args);

    void update(UpdateContext& ctx)
    {
        LOG_DEBUG("SystemManager::update called, systems count = {}", this->systems.size());
        
        using hrclock = std::chrono::high_resolution_clock;
        using milliseconds = std::chrono::milliseconds;

        for (const auto& system : this->systems)
        {
            auto start = hrclock::now();
            system->update(ctx);
            auto end = hrclock::now();
            auto ms = std::chrono::duration_cast<milliseconds>(end - start).count();
            if (ms > 5) LOG_DEBUG("System (type: {}) took {} ms", typeid(*system).name(), ms);
        }
    }

    int size() const { return static_cast<int>(this->systems.size()); }

private:
    std::vector<std::unique_ptr<System>> systems;
};

#include "SystemManager.inl"

#endif // system_manager_h
