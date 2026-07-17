#include "../../src/game/include/AnimationStateSystem/AnimationStateSystem.h"

#include "../../src/domain/components/AnimationComponent.h"
#include "../../src/domain/components/AnimationControllerComponent.h"
#include "../../src/domain/components/OrientationComponent.h"
#include "../../src/domain/events/OrientationChangedEvent.h"
#include "../../src/domain/include/World/World.h"
#include "../../src/domain/value_objects/Animation/Animation.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/value_objects/UpdateContext/UpdateContext.h"

#include "../../src/game/events/StateChangedEvent.h"

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
    }

    static Animation makeAnimation(int x) { return Animation{{{x, 0, 16, 16}}, 0.1f, true}; }

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
    const auto entity = this->world.entities().create();

    const Animation idleAnimation = this->makeAnimation(0);
    const Animation runningAnimation = this->makeAnimation(16);

    AnimationComponent animation;
    animation.animation = idleAnimation;
    animation.elapsedTime = 0.25f;
    animation.currentFrame = 3;
    animation.currentState = StateId::Idle;

    AnimationControllerComponent controller;
    controller.currentState = StateId::Idle;
    controller.animations.right[StateId::Idle] = idleAnimation;
    controller.animations.right[StateId::Running] = runningAnimation;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animation);
    comp.add<AnimationControllerComponent>(entity, controller);

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Idle, StateId::Running });
    this->system.update(this->context);

    const auto& updatedAnimation = comp.get<AnimationComponent>(entity);
    const auto& updatedController = comp.get<AnimationControllerComponent>(entity);

    REQUIRE(updatedController.currentState == StateId::Running);
    REQUIRE(updatedAnimation.currentState == StateId::Running);
    REQUIRE(updatedAnimation.currentFrame == 0);
    REQUIRE(updatedAnimation.elapsedTime == 0.f);
    REQUIRE(updatedAnimation.animation.frames.size() == 1);
    REQUIRE(updatedAnimation.animation.frames[0].x == 16);
}

TEST_CASE_METHOD(AnimationStateSystemFixture,
    "AnimationStateSystem ignores state change for entity without animation controller",
    "[integration][animation_state_system]"
) {
    const auto entity = this->world.entities().create();

    AnimationComponent animation;
    animation.animation = this->makeAnimation(0);
    animation.elapsedTime = 0.4f;
    animation.currentFrame = 2;
    animation.currentState = StateId::Idle;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animation);

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Idle, StateId::Running });
    this->system.update(this->context);

    const auto& unchanged = comp.get<AnimationComponent>(entity);
    REQUIRE(unchanged.currentState == StateId::Idle);
    REQUIRE(unchanged.currentFrame == 2);
    REQUIRE(unchanged.elapsedTime == 0.4f);
}

TEST_CASE_METHOD(AnimationStateSystemFixture,
    "AnimationStateSystem updates controller state but keeps animation when target state is not mapped",
    "[integration][animation_state_system]"
) {
    const auto entity = this->world.entities().create();

    const Animation idleAnimation = this->makeAnimation(0);

    AnimationComponent animation;
    animation.animation = idleAnimation;
    animation.elapsedTime = 0.3f;
    animation.currentFrame = 4;
    animation.currentState = StateId::Idle;

    AnimationControllerComponent controller;
    controller.currentState = StateId::Idle;
    controller.animations.right[StateId::Idle] = idleAnimation;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animation);
    comp.add<AnimationControllerComponent>(entity, controller);

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Idle, StateId::Running });
    this->system.update(this->context);

    const auto& updatedAnimation = comp.get<AnimationComponent>(entity);
    const auto& updatedController = comp.get<AnimationControllerComponent>(entity);

    REQUIRE(updatedController.currentState == StateId::Running);
    REQUIRE(updatedAnimation.currentState == StateId::Idle);
    REQUIRE(updatedAnimation.currentFrame == 4);
    REQUIRE(updatedAnimation.elapsedTime == 0.3f);
    REQUIRE(updatedAnimation.animation.frames[0].x == 0);
}

TEST_CASE_METHOD(AnimationStateSystemFixture,
    "AnimationStateSystem keeps progress when animation and controller are already in same state",
    "[integration][animation_state_system]"
) {
    const auto entity = this->world.entities().create();

    const Animation runningAnimation = this->makeAnimation(16);

    AnimationComponent animation;
    animation.animation = runningAnimation;
    animation.elapsedTime = 0.35f;
    animation.currentFrame = 5;
    animation.currentState = StateId::Running;

    AnimationControllerComponent controller;
    controller.currentState = StateId::Running;
    controller.animations.right[StateId::Running] = runningAnimation;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animation);
    comp.add<AnimationControllerComponent>(entity, controller);

    this->system.update(this->context);

    const auto& unchanged = comp.get<AnimationComponent>(entity);
    REQUIRE(unchanged.currentState == StateId::Running);
    REQUIRE(unchanged.currentFrame == 5);
    REQUIRE(unchanged.elapsedTime == 0.35f);
    REQUIRE(unchanged.animation.frames[0].x == 16);
}

TEST_CASE_METHOD(AnimationStateSystemFixture, "AnimationStateSystem applies pending changes in order and uses last emitted state",
    "[integration][animation_state_system]"
) {
    const auto entity = this->world.entities().create();

    const Animation idleAnimation = this->makeAnimation(0);
    const Animation runningAnimation = this->makeAnimation(16);
    const Animation jumpingAnimation = this->makeAnimation(32);

    AnimationComponent animation;
    animation.animation = idleAnimation;
    animation.elapsedTime = 0.5f;
    animation.currentFrame = 6;
    animation.currentState = StateId::Idle;

    AnimationControllerComponent controller;
    controller.currentState = StateId::Idle;
    controller.animations.right[StateId::Idle] = idleAnimation;
    controller.animations.right[StateId::Running] = runningAnimation;
    controller.animations.right[StateId::Jumping] = jumpingAnimation;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animation);
    comp.add<AnimationControllerComponent>(entity, controller);

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Idle, StateId::Running });
    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Running, StateId::Jumping });
    this->system.update(this->context);

    const auto& updatedAnimation = comp.get<AnimationComponent>(entity);
    const auto& updatedController = comp.get<AnimationControllerComponent>(entity);

    REQUIRE(updatedController.currentState == StateId::Jumping);
    REQUIRE(updatedAnimation.currentState == StateId::Jumping);
    REQUIRE(updatedAnimation.currentFrame == 0);
    REQUIRE(updatedAnimation.elapsedTime == 0.f);
    REQUIRE(updatedAnimation.animation.frames[0].x == 32);
}

TEST_CASE_METHOD(AnimationStateSystemFixture,
    "AnimationStateSystem selects left animation when OrientationComponent indicates left direction",
    "[integration][animation_state_system]"
) {
    const auto entity = this->world.entities().create();

    const Animation rightIdleAnimation = this->makeAnimation(0);
    const Animation leftIdleAnimation = this->makeAnimation(50);
    const Animation rightRunningAnimation = this->makeAnimation(16);
    const Animation leftRunningAnimation = this->makeAnimation(66);

    AnimationComponent animation;
    animation.animation = rightIdleAnimation;
    animation.elapsedTime = 0.1f;
    animation.currentFrame = 1;
    animation.currentState = StateId::Idle;

    AnimationControllerComponent controller;
    controller.currentState = StateId::Idle;
    controller.animations.right[StateId::Idle] = rightIdleAnimation;
    controller.animations.right[StateId::Running] = rightRunningAnimation;
    controller.animations.left[StateId::Idle] = leftIdleAnimation;
    controller.animations.left[StateId::Running] = leftRunningAnimation;
    controller.animations.symmetric = false;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animation);
    comp.add<AnimationControllerComponent>(entity, controller);
    comp.add<OrientationComponent>(entity, OrientationComponent{ Orientation::Left });

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Idle, StateId::Running });
    this->system.update(this->context);

    const auto& updatedAnimation = comp.get<AnimationComponent>(entity);

    REQUIRE(updatedAnimation.currentState == StateId::Running);
    REQUIRE(updatedAnimation.animation.frames[0].x == 66);
}

TEST_CASE_METHOD(AnimationStateSystemFixture, "AnimationStateSystem resets animation when orientation changes",
    "[integration][animation_state_system]"
) {
    const auto entity = this->world.entities().create();

    const Animation rightIdleAnimation = this->makeAnimation(0);
    const Animation leftIdleAnimation = this->makeAnimation(50);

    AnimationComponent animation;
    animation.animation = rightIdleAnimation;
    animation.elapsedTime = 0.25f;
    animation.currentFrame = 3;
    animation.currentState = StateId::Idle;

    AnimationControllerComponent controller;
    controller.currentState = StateId::Idle;
    controller.animations.right[StateId::Idle] = rightIdleAnimation;
    controller.animations.left[StateId::Idle] = leftIdleAnimation;
    controller.animations.symmetric = false;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animation);
    comp.add<AnimationControllerComponent>(entity, controller);
    comp.add<OrientationComponent>(entity, OrientationComponent{ Orientation::Right });

    auto& orientation = comp.get<OrientationComponent>(entity);
    orientation.direction = Orientation::Left;
    
    this->bus.emit<OrientationChangedEvent>(OrientationChangedEvent{ entity, Orientation::Right, Orientation::Left });
    this->system.update(this->context);

    const auto& updatedAnimation = comp.get<AnimationComponent>(entity);

    REQUIRE(updatedAnimation.currentState == StateId::Idle);
    REQUIRE(updatedAnimation.currentFrame == 0);
    REQUIRE(updatedAnimation.elapsedTime == 0.f);
    REQUIRE(updatedAnimation.animation.frames[0].x == 50);
}

TEST_CASE_METHOD(AnimationStateSystemFixture, "AnimationStateSystem uses right animation when no left animation exists",
    "[integration][animation_state_system]"
) {
    const auto entity = this->world.entities().create();

    const Animation idleAnimation = this->makeAnimation(0);
    const Animation runningAnimation = this->makeAnimation(16);

    AnimationComponent animation;
    animation.animation = idleAnimation;
    animation.currentState = StateId::Idle;

    AnimationControllerComponent controller;
    controller.currentState = StateId::Idle;
    controller.animations.right[StateId::Idle] = idleAnimation;
    controller.animations.right[StateId::Running] = runningAnimation;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animation);
    comp.add<AnimationControllerComponent>(entity, controller);
    comp.add<OrientationComponent>(entity, OrientationComponent{ Orientation::Left });

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Idle, StateId::Running });
    this->system.update(this->context);

    const auto& updatedAnimation = comp.get<AnimationComponent>(entity);

    REQUIRE(updatedAnimation.currentState == StateId::Running);
    REQUIRE(updatedAnimation.animation.frames[0].x == 16);
}

TEST_CASE_METHOD(AnimationStateSystemFixture, "AnimationStateSystem uses right animation when entity has no OrientationComponent",
    "[integration][animation_state_system]"
) {
    const auto entity = this->world.entities().create();

    const Animation idleAnimation = this->makeAnimation(0);
    const Animation runningAnimation = this->makeAnimation(16);

    AnimationComponent animation;
    animation.animation = idleAnimation;
    animation.currentState = StateId::Idle;

    AnimationControllerComponent controller;
    controller.currentState = StateId::Idle;
    controller.animations.right[StateId::Idle] = idleAnimation;
    controller.animations.right[StateId::Running] = runningAnimation;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animation);
    comp.add<AnimationControllerComponent>(entity, controller);

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Idle, StateId::Running });
    this->system.update(this->context);

    const auto& updatedAnimation = comp.get<AnimationComponent>(entity);

    REQUIRE(updatedAnimation.currentState == StateId::Running);
    REQUIRE(updatedAnimation.animation.frames[0].x == 16);
}
