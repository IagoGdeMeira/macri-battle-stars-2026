#include "../../src/game/include/AnimationSystem/AnimationSystem.h"

#include "../../src/domain/components/AnimationComponent.h"
#include "../../src/domain/components/SpriteComponent.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <vector>

class AnimationSystemFixture
{
public:
    AnimationSystemFixture() : system(), context{ world, bus, commandBuffer, 0.0f }
    {
        this->world.components().registerComponent<AnimationComponent>();
        this->world.components().registerComponent<SpriteComponent>();
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

    this->world.components().add<AnimationComponent>(
        entity,
        AnimationComponent{
            Animation{
                { { 0, 0, 16, 16 }, { 16, 0, 16, 16 } },
                0.1f,
                true
            },
            0.0f,
            0
        });

    this->world.components().add<SpriteComponent>(entity, SpriteComponent{ std::make_shared<Texture>(), 16, 16 });

    this->context.deltaTime = 0.1f;
    this->system.update(this->context);

    auto& animation = this->world.components().get<AnimationComponent>(entity);
    auto& sprite = this->world.components().get<SpriteComponent>(entity);

    REQUIRE(animation.currentFrame == 1);
    REQUIRE(animation.elapsedTime == 0.0f);
    REQUIRE(sprite.useSourceRect == true);
    REQUIRE(sprite.srcX == 16);
    REQUIRE(sprite.srcY == 0);
    REQUIRE(sprite.srcWidth == 16);
    REQUIRE(sprite.srcHeight == 16);
}

TEST_CASE_METHOD(AnimationSystemFixture,
    "AnimationSystem loops back to the first frame when configured to loop",
    "[unit][animation_system]"
) {
    const auto entity = this->world.entities().create();

    this->world.components().add<AnimationComponent>(entity,
        AnimationComponent { Animation{{{0, 0, 16, 16}, {16, 0, 16, 16}}, 0.1f, true}, 0.0f, 1 });

    this->world.components().add<SpriteComponent>(
        entity, SpriteComponent{ std::make_shared<Texture>(), 16, 16 });

    this->context.deltaTime = 0.1f;
    this->system.update(this->context);

    auto& animation = this->world.components().get<AnimationComponent>(entity);
    auto& sprite = this->world.components().get<SpriteComponent>(entity);

    REQUIRE(animation.currentFrame == 0);
    REQUIRE(sprite.srcX == 0);
    REQUIRE(sprite.srcY == 0);
    REQUIRE(sprite.srcWidth == 16);
    REQUIRE(sprite.srcHeight == 16);
}

TEST_CASE_METHOD(AnimationSystemFixture, "AnimationSystem holds the last frame when loop is disabled",
    "[unit][animation_system]"
) {
    const auto entity = this->world.entities().create();

    this->world.components().add<AnimationComponent>(entity,
        AnimationComponent{Animation {{{0, 0, 16, 16}, {16, 0, 16, 16}}, 0.1f, false}, 0.0f, 1});

    this->world.components().add<SpriteComponent>(entity, SpriteComponent{ std::make_shared<Texture>(), 16, 16 });

    this->context.deltaTime = 0.3f;
    this->system.update(this->context);

    auto& animation = this->world.components().get<AnimationComponent>(entity);
    auto& sprite = this->world.components().get<SpriteComponent>(entity);

    REQUIRE(animation.currentFrame == 1);
    REQUIRE(sprite.srcX == 16);
    REQUIRE(sprite.srcY == 0);
    REQUIRE(sprite.srcWidth == 16);
    REQUIRE(sprite.srcHeight == 16);
}
