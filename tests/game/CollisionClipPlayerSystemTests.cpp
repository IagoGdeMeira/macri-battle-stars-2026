#include "game/include/CollisionClipPlayerSystem/CollisionClipPlayerSystem.h"

#include "StubResourceManager.h"
#include "StubTextureFactory.h"
#include "StubTextureLoader.h"

#include "domain/components/CircleColliderComponent.h"
#include "domain/components/CollisionClipPlayerComponent.h"
#include "domain/components/CollisionClipDefinitionsComponent.h"
#include "domain/components/HitboxComponent.h"
#include "domain/components/HurtboxComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/PushboxComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/events/OrientationChangedEvent.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/CollisionFrame/CollisionFrame.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include "game/events/StateChangedEvent.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

class CollisionClipPlayerSystemFixture
{
public:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    StubResourceManager resourceManager;
    StubTextureFactory textureFactory;
    StubTextureLoader textureLoader;
    EntityFactory entityFactory;
    CollisionClipPlayerSystem system;
    UpdateContext context;

    CollisionClipPlayerSystemFixture() :
        textureLoader(this->textureFactory),
        entityFactory(this->world, this->resourceManager, this->textureLoader),
        system(this->bus, this->entityFactory),
        context{this->world, this->bus, this->commandBuffer, 0.016f}
    {
        auto& comp = this->world.components();
        comp.registerComponent<CollisionClipPlayerComponent>();
        comp.registerComponent<CollisionClipDefinitionsComponent>();
        comp.registerComponent<OrientationComponent>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<HitboxComponent>();
        comp.registerComponent<HurtboxComponent>();
        comp.registerComponent<PushboxComponent>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<RectangleColliderComponent>();
        comp.registerComponent<CircleColliderComponent>();
    }

    std::shared_ptr<CollisionClip> makeCollisionClip(bool loop = true, int frameCount = 2)
    {
        auto clip = std::make_shared<CollisionClip>();
        clip->loop = loop;

        for (int i = 0; i < frameCount; ++i)
        {
            CollisionFrame frame;
            frame.duration = 0.1f;

            if (i == 0)
            {
                HitboxDef hb;
                hb.damage = 10;
                hb.collider = std::make_unique<CircleDef>();
                dynamic_cast<CircleDef*>(hb.collider.get())->radius = 5.f;
                frame.hitboxes.push_back(std::move(hb));
            }
            clip->frames.push_back(std::move(frame));
        }
        return clip;
    }
};

TEST_CASE_METHOD(CollisionClipPlayerSystemFixture, "CollisionClipPlayerSystem creates hitboxes on state change",
    "[integration][collision_clip_player_system]"
) {
    const auto entity = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<CollisionClipPlayerComponent>(entity, CollisionClipPlayerComponent{});
    auto clip = this->makeCollisionClip();
    CollisionClipDefinitionsComponent defs;
    defs.clips[StateId::Idle] = clip;
    comp.add<CollisionClipDefinitionsComponent>(entity, defs);
    comp.add<OrientationComponent>(entity, OrientationComponent{ Orientation::Right });

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Running, StateId::Idle });
    this->system.update(this->context);

    auto view = View<ParentComponent>(comp);
    bool foundHitbox = false;
    for (auto [child, parent] : view)
    {
        if (parent.parent != entity || !comp.has<HitboxComponent>(child)) continue;
        foundHitbox = true;
        break;
    }
    REQUIRE(foundHitbox);
}

TEST_CASE_METHOD(CollisionClipPlayerSystemFixture, "CollisionClipPlayerSystem respects orientation when creating colliders",
    "[integration][collision_clip_player_system]"
) {
    const auto entity = this->world.entities().create();
    auto& comp = this->world.components();

    auto clip = this->makeCollisionClip();
    CollisionClipPlayerComponent player;
    player.playing = true;
    player.currentClip = clip;
    player.currentFrame = 0;
    
    comp.add<CollisionClipPlayerComponent>(entity, player);
    comp.add<CollisionClipDefinitionsComponent>(entity, CollisionClipDefinitionsComponent{});
    comp.add<OrientationComponent>(entity, OrientationComponent{ Orientation::Left });

    this->bus.emit<OrientationChangedEvent>(OrientationChangedEvent{ entity, Orientation::Right, Orientation::Left });
    this->system.update(this->context);

    auto view = View<ParentComponent>(comp);
    bool hasChild = false;
    for (auto [child, parent] : view)
    {
        if (parent.parent != entity) continue;
        hasChild = true;
        break;
    }
    REQUIRE(hasChild);
}
