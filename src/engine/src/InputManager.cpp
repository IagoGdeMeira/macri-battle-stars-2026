#include "InputManager/InputManager.h"

void InputManager::setProvider(std::unique_ptr<IPlatformEventProvider> newProvider)
{ this->provider = std::move(newProvider); }

void InputManager::addAdapter(std::unique_ptr<InputAdapter> adapter)
{ this->adapters.push_back(std::move(adapter)); }

void InputManager::poll()
{
    if (!this->provider) return;

    auto events = this->provider->pollEvents();
    for (auto& a : this->adapters) a->processEvents(events);
}
