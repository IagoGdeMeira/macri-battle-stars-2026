#include "StateMachineRegistry/StateMachineRegistry.h"

uint32_t StateMachineRegistry::registerMachine(StateMachine&& machine)
{
    uint32_t id;

    if (!this->freeIds.empty()) 
    {
        id = this->freeIds.front();
        this->freeIds.pop();
    }
    else id = this->nextId++;

    if (id >= this->machines.size()) this->machines.resize(id + 1);
    
    this->machines[id] = std::move(machine);
    return id;
}

void StateMachineRegistry::unregisterMachine(uint32_t id)
{
    if (id >= this->machines.size() || !this->machines[id].has_value()) return;

    this->machines[id].reset();
    this->freeIds.push(id);
}

const StateMachine* StateMachineRegistry::getMachine(uint32_t id) const
{
    if (id >= this->machines.size() || !this->machines[id].has_value()) return nullptr;
    return &this->machines[id].value();
}

bool StateMachineRegistry::hasMachine(uint32_t id) const
{ return id < this->machines.size() && this->machines[id].has_value(); }

void StateMachineRegistry::clear()
{
    this->machines.clear();
    this->freeIds = std::queue<uint32_t>();
    this->nextId = 0;
}
