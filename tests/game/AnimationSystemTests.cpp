#include "../../src/game/include/AnimationSystem/AnimationSystem.h"

#include "../stubs/StubTexture.h"

#include "../../src/domain/components/AnimationComponent.h"
#include "../../src/domain/components/SpriteComponent.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
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
    StubTexture stubTexture;
};

TEST_CASE_METHOD(AnimationSystemFixture, "AnimationSystem advances sprite source rect by frame duration",
    "[unit][animation_system]"
) {
    const auto entity = this->world.entities().create();

    auto& comp = this->world.components();
    comp.add<AnimationComponent>(entity, AnimationComponent{ Animation{{{0, 0, 16, 16}, {16, 0, 16, 16}}, 0.1f, true}, 0.f, 0 });
    comp.add<SpriteComponent>(entity, SpriteComponent{ std::make_shared<StubTexture>(), 16, 16 });

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
    comp.add<SpriteComponent>(entity, SpriteComponent{ std::make_shared<StubTexture>(), 16, 16 });

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
    comp.add<AnimationComponent>(entity, AnimationComponent{ Animation {{{0, 0, 16, 16}, {16, 0, 16, 16}}, 0.1f, false}, 0.f, 1 });
    comp.add<SpriteComponent>(entity, SpriteComponent{ std::make_shared<StubTexture>(), 16, 16 });

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
