#ifndef combo_system_h
#define combo_system_h

#include "../Combo/Combo.h"

#include "../../domain/components/InputBufferComponent.h"

#include "../../engine/include/System/System.h"
#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/value_objects/UpdateContext/UpdateContext.h"

#include <vector>

class ComboSystem : public System
{
public:
    ComboSystem(EventBus& bus, const std::vector<Combo>& combos) :
        eventBus(bus), combos(combos) {}

    void update(UpdateContext& ctx) override;

private:
    EventBus& eventBus;
    std::vector<Combo> combos;

    bool matches(const InputBufferComponent& buffer, const Combo& combo);
};

#endif // combo_system_h
