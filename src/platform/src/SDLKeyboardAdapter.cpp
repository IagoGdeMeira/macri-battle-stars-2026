#include "../include/SDLKeyboardAdapter/SDLKeyboardAdapter.h"

#include "../include/SDLKeyTranslator/SDLKeyTranslator.h"

#include "../../engine/events/InputEvent.h"
#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/InputContext/InputContext.h"

#include <SDL.h>

SDLKeyboardAdapter::SDLKeyboardAdapter(EventBus& eventBus, const InputContext& inputContext) :
    eventBus(eventBus) { this->buildKeyPlayerMap(inputContext); }
    
void SDLKeyboardAdapter::processEvents(const std::vector<std::unique_ptr<PlatformEvent>>& events)
{
    for (const auto& e : events)
    {
        if (e->type() != PlatformEvent::Type::Keyboard) continue;

        auto keyEvent = static_cast<const KeyboardEvent*>(e.get());
        
        auto it = this->keyToPlayer.find(static_cast<KeyCode>(keyEvent->keyCode));
        if (it == this->keyToPlayer.end()) continue;
        
        InputSource src = InputSource::keyboard(static_cast<KeyCode>(keyEvent->keyCode));
        auto digitalEvent = DigitalInputEvent(src, it->second, keyEvent->pressed);
        this->eventBus.emit<DigitalInputEvent>(digitalEvent);
    }
}

void SDLKeyboardAdapter::buildKeyPlayerMap(const InputContext& context)
{
    for (const auto& [id, b] : context.bindings) for (const auto& [src, _] : b.keyMap)
    {
        if (src.type() != InputSource::Type::Keyboard) continue;
        
        KeyCode key = static_cast<KeyCode>(src.code());
        this->keyToPlayer[key] = id;
    }
}
