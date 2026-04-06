#include "../../src/game/include/StateSystem/StateSystem.h"

#include "../../src/domain/components/StateComponent.h"
#include "../../src/domain/include/World/World.h"
#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"
#include "../../src/game/conditions/MinTimeCondition.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>

class StateSystemFixture
{
public:
    StateMachine makeMachineWithTrigger() const
    {
        StateMachine machine;

        StateTransition transition;
        transition.from = StateId::Idle;
        transition.to = StateId::Punching;
        transition.triggers.push_back(TriggerId::Punched);

        machine.transitions.push_back(std::move(transition));
        return machine;
    }
};

TEST_CASE_METHOD(StateSystemFixture, "StateSystem applies matching transition",
    "[integration][state_system]"
) {
    EventBus bus;
    StateMachine machine = this->makeMachineWithTrigger();
    StateSystem system(bus, machine);

    World world;
    world.components().registerComponent<StateComponent>();

    const auto entity = world.entities().create();
    world.components().add<StateComponent>(entity, StateComponent{});

    CommandBuffer commandBuffer;
    UpdateContext ctx { world, bus, commandBuffer, 0.016f };

    bus.emit<TriggerEvent>(TriggerEvent{ entity, TriggerId::Punched });
    system.update(ctx);

    const auto& state = world.components().get<StateComponent>(entity);

    REQUIRE(state.current == StateId::Punching);
    REQUIRE(state.timeInState == 0.0f);
}

TEST_CASE_METHOD(StateSystemFixture, "StateSystem keeps other entities unchanged",
    "[integration][state_system]"
) {
    EventBus bus;
    StateMachine machine = this->makeMachineWithTrigger();
    StateSystem system(bus, machine);

    World world;
    world.components().registerComponent<StateComponent>();

    const auto target = world.entities().create();
    const auto other = world.entities().create();

    world.components().add<StateComponent>(target, StateComponent{});
    world.components().add<StateComponent>(other, StateComponent{});

    CommandBuffer commandBuffer;
    UpdateContext ctx { world, bus, commandBuffer, 0.016f };

    bus.emit<TriggerEvent>(TriggerEvent{ other, TriggerId::Punched });
    system.update(ctx);

    const auto& targetState = world.components().get<StateComponent>(target);
    const auto& otherState = world.components().get<StateComponent>(other);

    REQUIRE(targetState.current == StateId::Idle);
    REQUIRE(otherState.current == StateId::Punching);
}

TEST_CASE_METHOD(StateSystemFixture, "StateSystem respects transition conditions",
    "[integration][state_system]"
) {
    EventBus bus;
    StateMachine machine;

    StateTransition transition;
    transition.from = StateId::Idle;
    transition.to = StateId::Punching;
    transition.triggers.push_back(TriggerId::Punched);
    transition.conditions.push_back(std::make_unique<MinTimeCondition>(0.5f));
    machine.transitions.push_back(std::move(transition));

    StateSystem system(bus, machine);

    World world;
    world.components().registerComponent<StateComponent>();

    const auto entity = world.entities().create();

    StateComponent state;
    state.timeInState = 0.25f;
    world.components().add<StateComponent>(entity, state);

    CommandBuffer commandBuffer;
    UpdateContext ctx { world, bus, commandBuffer, 0.10f };

    bus.emit<TriggerEvent>(TriggerEvent{ entity, TriggerId::Punched });
    system.update(ctx);

    const auto& updated = world.components().get<StateComponent>(entity);

    REQUIRE(updated.current == StateId::Idle);
    REQUIRE(updated.timeInState == 0.35f);
}
