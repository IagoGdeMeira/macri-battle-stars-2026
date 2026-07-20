#ifndef quit_action_h
#define quit_action_h

#include "IUIAction/IUIAction.h"

#include "engine/events/QuitEvent.h"
#include "engine/include/EventBus/EventBus.h"

class QuitAction : public IUIAction
{
public:
    explicit QuitAction(EventBus& bus) : bus(bus) {}
    void execute() const override { this->bus.emit<QuitEvent>(); }

private:
    EventBus& bus;
};

#endif // quit_action_h
