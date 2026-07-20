#ifndef state_machine_registry_h
#define state_machine_registry_h

#include "StateMachine/StateMachine.h"

#include <cstdint>
#include <optional>
#include <queue>
#include <vector>

class StateMachineRegistry
{
public:
    uint32_t registerMachine(StateMachine&& machine);
    void unregisterMachine(uint32_t id);
    const StateMachine* getMachine(uint32_t id) const;
    bool hasMachine(uint32_t id) const;
    void clear();

private:
    static constexpr char DEAD = 0, ALIVE = 1;

    uint32_t nextId = 0;
    std::queue<uint32_t> freeIds;
    std::vector<std::optional<StateMachine>> machines;
};

#endif // state_machine_registry_h
