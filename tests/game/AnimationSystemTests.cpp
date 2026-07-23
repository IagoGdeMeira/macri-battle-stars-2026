#include "game/include/AnimationSystem/AnimationSystem.h"

#include "domain/components/AnimationComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_test_macros.hpp>
#include <vector>

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

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, AnimationComponent{ Animation{{{0, 0, 16, 16}, {16, 0, 16, 16}}, 0.1f, true}, 0.f, 0 });
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

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, AnimationComponent { Animation{{{0, 0, 16, 16}, {16, 0, 16, 16}}, 0.1f, true}, 0.f, 1 });
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

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, AnimationComponent{ Animation{{{0, 0, 16, 16}, {16, 0, 16, 16}}, 0.1f, false}, 0.f, 1 });
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
