#include "game/include/AnimationStateSystem/AnimationStateSystem.h"

#include "domain/components/AnimationComponent.h"
#include "domain/components/AnimationControllerComponent.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/Animation/Animation.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include "game/events/StateChangedEvent.h"
#include "game/events/OrientationChangedEvent.h"

#include <catch2/catch_test_macros.hpp>

class AnimationStateSystemFixture
{
public:
    AnimationStateSystemFixture() : system(this->bus), context{ this->world, this->bus, this->commandBuffer, 0.016f }
    {
        auto& comp = this->world.components();
        comp.registerComponent<AnimationComponent>();
        comp.registerComponent<AnimationControllerComponent>();
        comp.registerComponent<OrientationComponent>();
        comp.registerComponent<ChildrenComponent>();
        comp.registerComponent<ParentComponent>();
    }

    static Animation makeAnimation(int x) { return Animation{{{x, 0, 16, 16}}, 0.1f, true}; }

    std::pair<Entity, Entity> createControlledEntity()
    {
        auto& comp = this->world.components();
        auto& entities = this->world.entities();

        Entity parent = entities.create();
        comp.add<AnimationControllerComponent>(parent, AnimationControllerComponent{});

        Entity child = entities.create();
        comp.add<AnimationComponent>(child, AnimationComponent{});
        comp.add<ParentComponent>(child, ParentComponent{parent});

        if (!comp.has<ChildrenComponent>(parent)) comp.add<ChildrenComponent>(parent, ChildrenComponent{});
        comp.get<ChildrenComponent>(parent).children.push_back(child);

        return {parent, child};
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    AnimationStateSystem system;
    UpdateContext context;
};

TEST_CASE_METHOD(AnimationStateSystemFixture, "AnimationStateSystem applies pending state change and resets animation progress",
    "[integration][animation_state_system]"
) {
    auto [parent, child] = this->createControlledEntity();
    auto& comp = this->world.components();

    Animation idleAnim = this->makeAnimation(0);
    Animation runningAnim = this->makeAnimation(16);

    auto& ctrl = comp.get<AnimationControllerComponent>(parent);
    ctrl.animations.right[StateId::Idle] = idleAnim;
    ctrl.animations.right[StateId::Running] = runningAnim;
    ctrl.currentState = StateId::Idle;

    auto& anim = comp.get<AnimationComponent>(child);
    anim.currentState = StateId::Idle;
    anim.elapsedTime = 0.25f;
    anim.currentFrame = 3;
    anim.currentAnimation = idleAnim;

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ parent, StateId::Idle, StateId::Running });
    this->system.update(this->context);

    const auto& updatedAnim = comp.get<AnimationComponent>(child);
    const auto& updatedCtrl = comp.get<AnimationControllerComponent>(parent);

    REQUIRE(updatedCtrl.currentState == StateId::Running);
    REQUIRE(updatedAnim.currentState == StateId::Running);
    REQUIRE(updatedAnim.currentFrame == 0);
    REQUIRE(updatedAnim.elapsedTime == 0.f);
}

TEST_CASE_METHOD(AnimationStateSystemFixture, "AnimationStateSystem ignores state change for entity without animation controller",
    "[integration][animation_state_system]"
) {
    Entity standalone = this->world.entities().create();
    auto& comp = this->world.components();
    AnimationComponent anim;
    anim.currentState = StateId::Idle;
    anim.elapsedTime = 0.4f;
    anim.currentFrame = 2;
    comp.add<AnimationComponent>(standalone, anim);

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ standalone, StateId::Idle, StateId::Running });
    this->system.update(this->context);

    const auto& unchanged = comp.get<AnimationComponent>(standalone);
    REQUIRE(unchanged.currentState == StateId::Idle);
    REQUIRE(unchanged.currentFrame == 2);
    REQUIRE(unchanged.elapsedTime == 0.4f);
}

TEST_CASE_METHOD(AnimationStateSystemFixture,
    "AnimationStateSystem updates controller state but keeps animation when target state is not mapped",
    "[integration][animation_state_system]"
) {
    auto [parent, child] = this->createControlledEntity();
    auto& comp = this->world.components();

    Animation idleAnim = this->makeAnimation(0);
    auto& ctrl = comp.get<AnimationControllerComponent>(parent);
    ctrl.animations.right[StateId::Idle] = idleAnim;
    ctrl.currentState = StateId::Idle;

    auto& anim = comp.get<AnimationComponent>(child);
    anim.currentState = StateId::Idle;
    anim.currentFrame = 4;
    anim.elapsedTime = 0.3f;
    anim.currentAnimation = idleAnim;

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ parent, StateId::Idle, StateId::Running });
    this->system.update(this->context);

    const auto& updatedAnim = comp.get<AnimationComponent>(child);
    const auto& updatedCtrl = comp.get<AnimationControllerComponent>(parent);

    REQUIRE(updatedCtrl.currentState == StateId::Running);
    REQUIRE(updatedAnim.currentState == StateId::Idle);
    REQUIRE(updatedAnim.currentFrame == 4);
    REQUIRE(updatedAnim.elapsedTime == 0.3f);
}

TEST_CASE_METHOD(AnimationStateSystemFixture,
    "AnimationStateSystem keeps progress when animation and controller are already in same state",
    "[integration][animation_state_system]"
) {
    auto [parent, child] = this->createControlledEntity();
    auto& comp = this->world.components();

    Animation runningAnim = this->makeAnimation(16);
    auto& ctrl = comp.get<AnimationControllerComponent>(parent);
    ctrl.animations.right[StateId::Running] = runningAnim;
    ctrl.currentState = StateId::Running;

    auto& anim = comp.get<AnimationComponent>(child);
    anim.currentState = StateId::Running;
    anim.currentFrame = 5;
    anim.elapsedTime = 0.35f;
    anim.currentAnimation = runningAnim;

    this->system.update(this->context);

    const auto& unchanged = comp.get<AnimationComponent>(child);
    REQUIRE(unchanged.currentState == StateId::Running);
    REQUIRE(unchanged.currentFrame == 5);
    REQUIRE(unchanged.elapsedTime == 0.35f);
}

TEST_CASE_METHOD(AnimationStateSystemFixture, "AnimationStateSystem applies pending changes in order and uses last emitted state",
    "[integration][animation_state_system]"
) {
    auto [parent, child] = this->createControlledEntity();
    auto& comp = this->world.components();

    Animation idleAnim = this->makeAnimation(0);
    Animation runningAnim = this->makeAnimation(16);
    Animation jumpingAnim = this->makeAnimation(32);

    auto& ctrl = comp.get<AnimationControllerComponent>(parent);
    ctrl.animations.right[StateId::Idle] = idleAnim;
    ctrl.animations.right[StateId::Running] = runningAnim;
    ctrl.animations.right[StateId::Jumping] = jumpingAnim;
    ctrl.currentState = StateId::Idle;

    auto& anim = comp.get<AnimationComponent>(child);
    anim.currentState = StateId::Idle;
    anim.currentFrame = 6;
    anim.elapsedTime = 0.5f;
    anim.currentAnimation = idleAnim;

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ parent, StateId::Idle, StateId::Running });
    this->bus.emit<StateChangedEvent>(StateChangedEvent{ parent, StateId::Running, StateId::Jumping });
    this->system.update(this->context);

    const auto& updatedAnim = comp.get<AnimationComponent>(child);
    const auto& updatedCtrl = comp.get<AnimationControllerComponent>(parent);

    REQUIRE(updatedCtrl.currentState == StateId::Jumping);
    REQUIRE(updatedAnim.currentState == StateId::Jumping);
    REQUIRE(updatedAnim.currentFrame == 0);
    REQUIRE(updatedAnim.elapsedTime == 0.f);
}

TEST_CASE_METHOD(AnimationStateSystemFixture,
    "AnimationStateSystem selects left animation when OrientationComponent indicates left direction",
    "[integration][animation_state_system]"
) {
    auto [parent, child] = this->createControlledEntity();
    auto& comp = this->world.components();

    Animation rightIdle = this->makeAnimation(0);
    Animation leftIdle = this->makeAnimation(50);
    Animation rightRun = this->makeAnimation(16);
    Animation leftRun = this->makeAnimation(66);

    auto& ctrl = comp.get<AnimationControllerComponent>(parent);
    ctrl.animations.right[StateId::Idle] = rightIdle;
    ctrl.animations.right[StateId::Running] = rightRun;
    ctrl.animations.left[StateId::Idle] = leftIdle;
    ctrl.animations.left[StateId::Running] = leftRun;
    ctrl.animations.symmetric = false;
    ctrl.currentState = StateId::Idle;

    comp.add<OrientationComponent>(parent, OrientationComponent{ Orientation::Left });

    auto& anim = comp.get<AnimationComponent>(child);
    anim.currentState = StateId::Idle;
    anim.currentAnimation = rightIdle;

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ parent, StateId::Idle, StateId::Running });
    this->system.update(this->context);

    const auto& updatedAnim = comp.get<AnimationComponent>(child);
    REQUIRE(updatedAnim.currentState == StateId::Running);
    REQUIRE(updatedAnim.currentAnimation.frames[0].x == 66);
}

TEST_CASE_METHOD(AnimationStateSystemFixture, "AnimationStateSystem resets animation when orientation changes",
    "[integration][animation_state_system]"
) {
    auto [parent, child] = this->createControlledEntity();
    auto& comp = this->world.components();

    Animation rightIdle = this->makeAnimation(0);
    Animation leftIdle = this->makeAnimation(50);

    auto& ctrl = comp.get<AnimationControllerComponent>(parent);
    ctrl.animations.right[StateId::Idle] = rightIdle;
    ctrl.animations.left[StateId::Idle] = leftIdle;
    ctrl.animations.symmetric = false;
    ctrl.currentState = StateId::Idle;

    comp.add<OrientationComponent>(parent, OrientationComponent{ Orientation::Right });

    auto& anim = comp.get<AnimationComponent>(child);
    anim.currentState = StateId::Idle;
    anim.currentAnimation = rightIdle;
    anim.elapsedTime = 0.25f;
    anim.currentFrame = 3;

    auto& orient = comp.get<OrientationComponent>(parent);
    orient.direction = Orientation::Left;
    this->bus.emit<OrientationChangedEvent>(OrientationChangedEvent{ parent, Orientation::Right, Orientation::Left });
    this->system.update(this->context);

    const auto& updatedAnim = comp.get<AnimationComponent>(child);
    REQUIRE(updatedAnim.currentState == StateId::Idle);
    REQUIRE(updatedAnim.currentFrame == 0);
    REQUIRE(updatedAnim.elapsedTime == 0.f);
    REQUIRE(updatedAnim.currentAnimation.frames[0].x == 50);
}

TEST_CASE_METHOD(AnimationStateSystemFixture, "AnimationStateSystem uses right animation when no left animation exists",
    "[integration][animation_state_system]"
) {
    auto [parent, child] = this->createControlledEntity();
    auto& comp = this->world.components();

    Animation idleAnim = this->makeAnimation(0);
    Animation runningAnim = this->makeAnimation(16);

    auto& ctrl = comp.get<AnimationControllerComponent>(parent);
    ctrl.animations.right[StateId::Idle] = idleAnim;
    ctrl.animations.right[StateId::Running] = runningAnim;
    ctrl.currentState = StateId::Idle;

    comp.add<OrientationComponent>(parent, OrientationComponent{ Orientation::Left });

    auto& anim = comp.get<AnimationComponent>(child);
    anim.currentState = StateId::Idle;
    anim.currentAnimation = idleAnim;

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ parent, StateId::Idle, StateId::Running });
    this->system.update(this->context);

    const auto& updatedAnim = comp.get<AnimationComponent>(child);
    REQUIRE(updatedAnim.currentState == StateId::Running);
    REQUIRE(updatedAnim.currentAnimation.frames[0].x == 16);
}

TEST_CASE_METHOD(AnimationStateSystemFixture, "AnimationStateSystem uses right animation when entity has no OrientationComponent",
    "[integration][animation_state_system]"
) {
    auto [parent, child] = this->createControlledEntity();
    auto& comp = this->world.components();

    Animation idleAnim = this->makeAnimation(0);
    Animation runningAnim = this->makeAnimation(16);

    auto& ctrl = comp.get<AnimationControllerComponent>(parent);
    ctrl.animations.right[StateId::Idle] = idleAnim;
    ctrl.animations.right[StateId::Running] = runningAnim;
    ctrl.currentState = StateId::Idle;

    auto& anim = comp.get<AnimationComponent>(child);
    anim.currentState = StateId::Idle;
    anim.currentAnimation = idleAnim;

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ parent, StateId::Idle, StateId::Running });
    this->system.update(this->context);

    const auto& updatedAnim = comp.get<AnimationComponent>(child);
    REQUIRE(updatedAnim.currentState == StateId::Running);
    REQUIRE(updatedAnim.currentAnimation.frames[0].x == 16);
}
