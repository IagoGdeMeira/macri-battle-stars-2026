#include "InputManager/InputManager.h"

void InputManager::setProvider(Provider newProvider) { this->provider = std::move(newProvider); }

void InputManager::addAdapter(Adapter adapter) { this->adapters.push_back(std::move(adapter)); }

void InputManager::poll()
{
    if (!this->provider) return;

    auto events = this->provider->pollEvents();
    for (auto& a : this->adapters) a->processEvents(events);
}
