#include "CommandBuffer/CommandBuffer.h"

#include "domain/include/World/World.h"

#include <utility>

void CommandBuffer::create(std::function<void(World&)> cmd)
{ this->commands.push_back(std::move(cmd)); }

void CommandBuffer::destroy(Entity entity)
{ this->create([entity](World& world) { world.destroy(entity); }); }

void CommandBuffer::flush(World& world)
{
    std::vector<std::function<void(World&)>> current;
    current.swap(this->commands);

    for (size_t i = 0; i < current.size(); ++i)
    {
        try { current[i](world); }
        catch (...)
        {
            std::vector<std::function<void(World&)>> pending;
            pending.reserve(current.size() - i - 1 + this->commands.size());

            for (size_t next = i + 1; next < current.size(); ++next)
            { pending.push_back(std::move(current[next])); }

            for (auto& command : this->commands) pending.push_back(std::move(command));

            this->commands.swap(pending);
            throw;
        }
    }
}
