#include "../include/CommandBuffer/CommandBuffer.h"

#include "../../domain/include/World/World.h"

void CommandBuffer::create(std::function<void(World &)> cmd)
{ this->commands.push_back(std::move(cmd)); }

void CommandBuffer::flush(World& world)
{
    std::vector<std::function<void(World&)>> current;
    current.swap(this->commands);

    for (auto& cmd : current) cmd(world);
}
