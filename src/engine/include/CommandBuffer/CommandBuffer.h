#ifndef command_buffer_h
#define command_buffer_h

#include <functional>
#include <vector>

class World;

class CommandBuffer
{
public:
    void create(std::function<void(World &)> cmd);
    void flush(World &world);

private:
    std::vector<std::function<void(World &)>> commands;
};

#endif // command_buffer_h