#include "../../src/game/include/StateSystem/StateSystem.h"

#include "../../src/domain/components/StateComponent.h"
#include "../../src/domain/components/StateMachineComponent.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include "../../src/game/trigger_conditions/MinTimeCondition.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>

class StateSystemFixture
{
public:
    StateMachine makeMachineWithTrigger() const
    {
        StateMachine machine;

        StateTransition lowPriority;
        lowPriority.from = StateId::Idle;
        lowPriority.to = StateId::Punching;
        lowPriority.triggers.push_back(TriggerId::Punched);
        lowPriority.priority = 1;

        StateTransition highPriority;
        highPriority.from = StateId::Idle;
        highPriority.to = StateId::Blocking;
        highPriority.triggers.push_back(TriggerId::Punched);
        highPriority.priority = 10;

        machine.transitions.push_back(std::move(lowPriority));
        machine.transitions.push_back(std::move(highPriority));
        return machine;
    }

    void attachMachine(World& world, Entity entity, StateMachine machine) const
    { world.components().add<StateMachineComponent>(entity, StateMachineComponent{ std::move(machine) }); }
};

TEST_CASE_METHOD(StateSystemFixture, "StateSystem applies matching transition",
    "[integration][state_system]"
) {
    EventBus bus;
    StateSystem system(bus);

    World world;
    auto& components = world.components();
    components.registerComponent<StateComponent>();
    components.registerComponent<StateMachineComponent>();

    const auto entity = world.entities().create();
    components.add<StateComponent>(entity, StateComponent{});
    this->attachMachine(world, entity, this->makeMachineWithTrigger());

    CommandBuffer commandBuffer;
    UpdateContext ctx { world, bus, commandBuffer, 0.016f };

    bus.emit<TriggerEvent>(TriggerEvent{ entity, TriggerId::Punched });
    system.update(ctx);

    const auto& state = components.get<StateComponent>(entity);

    REQUIRE(state.current == StateId::Blocking);
    REQUIRE(state.timeInState == 0.f);
}

TEST_CASE_METHOD(StateSystemFixture, "StateSystem chooses the highest priority transition",
    "[integration][state_system]"
) {
    EventBus bus;
    StateSystem system(bus);

    auto machine = this->makeMachineWithTrigger();

    REQUIRE(machine.transitions.size() == 2);
    REQUIRE(machine.transitions[0].priority == 1);
    REQUIRE(machine.transitions[1].priority == 10);
    REQUIRE(machine.transitions[0].to == StateId::Punching);
    REQUIRE(machine.transitions[1].to == StateId::Blocking);

    World world;
    auto& components = world.components();
    components.registerComponent<StateComponent>();
    components.registerComponent<StateMachineComponent>();

    const auto entity = world.entities().create();
    components.add<StateComponent>(entity, StateComponent{});
    this->attachMachine(world, entity, std::move(machine));

    CommandBuffer commandBuffer;
    UpdateContext ctx { world, bus, commandBuffer, 0.016f };

    bus.emit<TriggerEvent>(TriggerEvent{ entity, TriggerId::Punched });
    system.update(ctx);

    const auto& state = components.get<StateComponent>(entity);

    REQUIRE(state.current == StateId::Blocking);
    REQUIRE(state.timeInState == 0.f);
}

TEST_CASE_METHOD(StateSystemFixture, "StateSystem applies only one transition per entity per frame",
    "[integration][state_system]"
) {
    EventBus bus;
    StateSystem system(bus);

    StateMachine machine;

    StateTransition first;
    first.from = StateId::Idle;
    first.to = StateId::Walking;
    first.triggers.push_back(TriggerId::Punched);
    first.priority = 5;

    StateTransition second;
    second.from = StateId::Walking;
    second.to = StateId::Running;
    second.triggers.push_back(TriggerId::Kicked);
    second.priority = 10;

    machine.transitions.push_back(std::move(first));
    machine.transitions.push_back(std::move(second));

    World world;
    auto& components = world.components();
    components.registerComponent<StateComponent>();
    components.registerComponent<StateMachineComponent>();

    const auto entity = world.entities().create();
    components.add<StateComponent>(entity, StateComponent{});
    this->attachMachine(world, entity, std::move(machine));

    CommandBuffer commandBuffer;
    UpdateContext ctx { world, bus, commandBuffer, 0.016f };

    bus.emit<TriggerEvent>(TriggerEvent{ entity, TriggerId::Punched });
    bus.emit<TriggerEvent>(TriggerEvent{ entity, TriggerId::Kicked });
    system.update(ctx);

    const auto& state = components.get<StateComponent>(entity);

    REQUIRE(state.current == StateId::Walking);
    REQUIRE(state.timeInState == 0.f);
}

TEST_CASE_METHOD(StateSystemFixture, "StateSystem keeps other entities unchanged",
    "[integration][state_system]"
) {
    EventBus bus;
    StateSystem system(bus);

    World world;
    auto& components = world.components();
    components.registerComponent<StateComponent>();
    components.registerComponent<StateMachineComponent>();

    const auto target = world.entities().create();
    const auto other = world.entities().create();

    components.add<StateComponent>(target, StateComponent{});
    components.add<StateComponent>(other, StateComponent{});
    this->attachMachine(world, target, this->makeMachineWithTrigger());
    this->attachMachine(world, other, this->makeMachineWithTrigger());

    CommandBuffer commandBuffer;
    UpdateContext ctx { world, bus, commandBuffer, 0.016f };

    bus.emit<TriggerEvent>(TriggerEvent{ other, TriggerId::Punched });
    system.update(ctx);

    const auto& targetState = components.get<StateComponent>(target);
    const auto& otherState = components.get<StateComponent>(other);

    REQUIRE(targetState.current == StateId::Idle);
    REQUIRE(otherState.current == StateId::Blocking);
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

    StateSystem system(bus);

    World world;
    auto& components = world.components();
    components.registerComponent<StateComponent>();
    components.registerComponent<StateMachineComponent>();

    const auto entity = world.entities().create();

    StateComponent state;
    state.timeInState = 0.25f;
    components.add<StateComponent>(entity, state);
    this->attachMachine(world, entity, std::move(machine));

    CommandBuffer commandBuffer;
    UpdateContext ctx { world, bus, commandBuffer, 0.1f };

    bus.emit<TriggerEvent>(TriggerEvent{ entity, TriggerId::Punched });
    system.update(ctx);

    const auto& updated = components.get<StateComponent>(entity);

    REQUIRE(updated.current == StateId::Idle);
    REQUIRE(updated.timeInState == 0.35f);
}
