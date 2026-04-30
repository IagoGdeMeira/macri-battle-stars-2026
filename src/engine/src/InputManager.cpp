#include "../include/InputManager/InputManager.h"

void InputManager::addAdapter(std::unique_ptr<InputAdapter> adapter)
{ this->adapters.push_back(std::move(adapter)); }

void InputManager::poll() { for (auto& adapter : this->adapters) adapter->poll(); }
