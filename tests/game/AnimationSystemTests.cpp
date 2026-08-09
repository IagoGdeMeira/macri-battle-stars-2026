#include "game/include/AnimationSystem/AnimationSystem.h"

#include "domain/components/AnimationComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class AnimationSystemFixture
{
public:
    AnimationSystemFixture() : system(), context{ this->world, this->bus, this->commandBuffer, 0.f }
    {
        auto& comp = this->world.components();
        comp.registerComponent<AnimationComponent>();
        comp.registerComponent<SpriteComponent>();
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    AnimationSystem system;
    UpdateContext context;
};

TEST_CASE_METHOD(AnimationSystemFixture, "AnimationSystem advances sprite source rect by frame duration",
    "[unit][animation_system]"
) {
    const auto entity = this->world.entities().create();

    Animation animData{ {{0, 0, 16, 16}, {16, 0, 16, 16}}, 0.1f, true };
    AnimationComponent animComp;
    animComp.currentAnimation = animData;
    animComp.elapsedTime = 0.f;
    animComp.currentFrame = 0;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animComp);
    comp.add<SpriteComponent>(entity, SpriteComponent{ "dummy.png", nullptr, Dimension2D{16, 16}, Rectangle{}, false });

    this->context.deltaTime = 0.1f;
    this->system.update(this->context);

    auto& animation = comp.get<AnimationComponent>(entity);
    auto& sprite = comp.get<SpriteComponent>(entity);

    REQUIRE(animation.currentFrame == 1);
    REQUIRE(animation.elapsedTime == 0.f);
    REQUIRE(sprite.useSourceRect == true);
    REQUIRE(sprite.source.position.x == 16);
    REQUIRE(sprite.source.position.y == 0);
    REQUIRE(sprite.source.size.width == 16);
    REQUIRE(sprite.source.size.height == 16);
}

TEST_CASE_METHOD(AnimationSystemFixture, "AnimationSystem loops back to the first frame when configured to loop",
    "[unit][animation_system]"
) {
    const auto entity = this->world.entities().create();

    Animation animData{ {{0, 0, 16, 16}, {16, 0, 16, 16}}, 0.1f, true };
    AnimationComponent animComp;
    animComp.currentAnimation = animData;
    animComp.elapsedTime = 0.f;
    animComp.currentFrame = 1;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animComp);
    comp.add<SpriteComponent>(entity, SpriteComponent{ "dummy.png", nullptr, Dimension2D{16, 16}, Rectangle{}, false });

    this->context.deltaTime = 0.1f;
    this->system.update(this->context);

    auto& animation = comp.get<AnimationComponent>(entity);
    auto& sprite = comp.get<SpriteComponent>(entity);

    REQUIRE(animation.currentFrame == 0);
    REQUIRE(sprite.source.position.x == 0);
    REQUIRE(sprite.source.position.y == 0);
    REQUIRE(sprite.source.size.width == 16);
    REQUIRE(sprite.source.size.height == 16);
}

TEST_CASE_METHOD(AnimationSystemFixture, "AnimationSystem holds the last frame when loop is disabled",
    "[unit][animation_system]"
) {
    const auto entity = this->world.entities().create();

    Animation animData{ {{0, 0, 16, 16}, {16, 0, 16, 16}}, 0.1f, false };
    AnimationComponent animComp;
    animComp.currentAnimation = animData;
    animComp.elapsedTime = 0.f;
    animComp.currentFrame = 1;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animComp);
    comp.add<SpriteComponent>(entity, SpriteComponent{ "dummy.png", nullptr, Dimension2D{16, 16}, Rectangle{}, false });

    this->context.deltaTime = 0.3f;
    this->system.update(this->context);

    auto& animation = comp.get<AnimationComponent>(entity);
    auto& sprite = comp.get<SpriteComponent>(entity);

    REQUIRE(animation.currentFrame == 1);
    REQUIRE(sprite.source.position.x == 16);
    REQUIRE(sprite.source.position.y == 0);
    REQUIRE(sprite.source.size.width == 16);
    REQUIRE(sprite.source.size.height == 16);
}

TEST_CASE_METHOD(AnimationSystemFixture, "AnimationSystem uses per-frame duration when specified",
    "[unit][animation_system]")
{
    const auto entity = this->world.entities().create();

    Animation anim;
    anim.frameDuration = 0.2f;
    anim.loop = true;
    anim.frames = {{0, 0, 16, 16, 0.2f}, {16, 0, 16, 16, 0.1f}, {32, 0, 16, 16, 0.f}};

    AnimationComponent animComp;
    animComp.currentAnimation = anim;
    animComp.elapsedTime = 0.f;
    animComp.currentFrame = 0;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animComp);
    comp.add<SpriteComponent>(entity, SpriteComponent{"tex", nullptr, Dimension2D{16,16}, Rectangle{}, false});

    this->context.deltaTime = 0.2f;
    this->system.update(this->context);
    auto& animComp1 = comp.get<AnimationComponent>(entity);
    REQUIRE(animComp1.currentFrame == 1);
    REQUIRE(animComp1.elapsedTime == 0.f);

    this->context.deltaTime = 0.1f;
    this->system.update(this->context);
    auto& animComp2 = comp.get<AnimationComponent>(entity);
    REQUIRE(animComp2.currentFrame == 2);
    REQUIRE(animComp2.elapsedTime == 0.f);

    this->context.deltaTime = 0.2f;
    this->system.update(this->context);
    auto& animComp3 = comp.get<AnimationComponent>(entity);
    REQUIRE(animComp3.currentFrame == 0);
    REQUIRE(animComp3.elapsedTime == 0.f);
}

TEST_CASE_METHOD(AnimationSystemFixture, "AnimationSystem falls back to global frameDuration when per-frame duration is 0",
    "[unit][animation_system]")
{
    const auto entity = this->world.entities().create();

    Animation anim;
    anim.frameDuration = 0.5f;
    anim.loop = false;
    anim.frames = {{0, 0, 16, 16, 0.f}, {16, 0, 16, 16, 0.f}};

    AnimationComponent animComp;
    animComp.currentAnimation = anim;
    animComp.currentFrame = 0;
    animComp.elapsedTime = 0.f;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animComp);
    comp.add<SpriteComponent>(entity, SpriteComponent{"tex", nullptr, Dimension2D{16,16}});

    this->context.deltaTime = 0.3f;
    this->system.update(this->context);
    REQUIRE(comp.get<AnimationComponent>(entity).currentFrame == 0);

    this->context.deltaTime = 0.3f;
    this->system.update(this->context);
    REQUIRE(comp.get<AnimationComponent>(entity).currentFrame == 1);
    REQUIRE(comp.get<AnimationComponent>(entity).elapsedTime == Catch::Approx(0.1f));
}

TEST_CASE_METHOD(AnimationSystemFixture, "AnimationSystem handles mixed durations with loop disabled",
    "[unit][animation_system]")
{
    const auto entity = this->world.entities().create();

    Animation anim;
    anim.frameDuration = 0.2f;
    anim.loop = false;
    anim.frames = {{0, 0, 16, 16, 0.05f}, {16, 0, 16, 16, 0.3f}, {32, 0, 16, 16, 0.f}};

    AnimationComponent animComp;
    animComp.currentAnimation = anim;
    animComp.currentFrame = 0;
    animComp.elapsedTime = 0.f;

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, animComp);
    comp.add<SpriteComponent>(entity, SpriteComponent{"tex", nullptr, Dimension2D{16,16}});

    this->context.deltaTime = 0.05f;
    this->system.update(this->context);
    REQUIRE(comp.get<AnimationComponent>(entity).currentFrame == 1);

    this->context.deltaTime = 0.3f;
    this->system.update(this->context);
    REQUIRE(comp.get<AnimationComponent>(entity).currentFrame == 2);

    this->context.deltaTime = 0.5f;
    this->system.update(this->context);
    auto& finalAnim = comp.get<AnimationComponent>(entity);
    REQUIRE(finalAnim.currentFrame == 2);
    REQUIRE(finalAnim.elapsedTime == 0.f);
}
