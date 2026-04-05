#ifndef condition_context_h
#define condition_context_h

#include "../../domain/components/StateComponent.h"
#include "../../domain/include/Entity/Entity.h"
#include "../../domain/include/World/World.h"

struct ConditionContext
{
    World& world;
    Entity& entity;
    StateComponent& stateComponent;
};

#endif // condition_context_h
