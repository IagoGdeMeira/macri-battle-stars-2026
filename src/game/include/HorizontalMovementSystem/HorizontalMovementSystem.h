#ifndef horizontal_movement_system_h
#define horizontal_movement_system_h

#include "../../domain/components/InputComponent.h"
#include "../../domain/value_objects/StateId/StateId.h"

#include "../../engine/include/System/System.h"

class HorizontalMovementSystem : public System
{
public:
    explicit HorizontalMovementSystem(float moveSpeed = 300.f) : moveSpeed(moveSpeed) {}

    void update(UpdateContext& ctx) override;

private:
    float moveSpeed;

    bool canMove(StateId state) const;
    bool hasInputAction(InputComponent& input, InputAction action) const;
};

#endif // horizontal_movement_system_h
