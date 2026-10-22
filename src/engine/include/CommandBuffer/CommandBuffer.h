#ifndef command_buffer_h
#define command_buffer_h

#include "domain/include/Entity/Entity.h"

#include <functional>
#include <vector>

class World;

class CommandBuffer
{
public:
    using Command = std::function<void(World&)>;

    void create(Command cmd);
    void destroy(Entity entity);
    void flush(World& world);

private:
    std::vector<Command> commands;
};

#endif // command_buffer_h
