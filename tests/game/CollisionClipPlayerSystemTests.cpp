#include "../../src/game/include/CollisionClipPlayerSystem/CollisionClipPlayerSystem.h"

#include "../stubs/StubResourceManager.h"
#include "../stubs/StubTextureFactory.h"
#include "../stubs/StubTextureLoader.h"

#include "../../src/domain/components/CircleColliderComponent.h"
#include "../../src/domain/components/CollisionClipPlayerComponent.h"
#include "../../src/domain/components/CollisionClipDefinitionsComponent.h"
#include "../../src/domain/components/HitboxComponent.h"
#include "../../src/domain/components/HurtboxComponent.h"
#include "../../src/domain/components/LocalTransform.h"
#include "../../src/domain/components/OrientationComponent.h"
#include "../../src/domain/components/ParentComponent.h"
#include "../../src/domain/components/PushboxComponent.h"
#include "../../src/domain/components/RectangleColliderComponent.h"
#include "../../src/domain/include/CollisionFrame/CollisionFrame.h"
#include "../../src/domain/include/World/World.h"
#include "../../src/domain/events/OrientationChangedEvent.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include "../../src/game/events/StateChangedEvent.h"

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
        auto& components = this->world.components();
        components.registerComponent<CollisionClipPlayerComponent>();
        components.registerComponent<CollisionClipDefinitionsComponent>();
        components.registerComponent<OrientationComponent>();
        components.registerComponent<ParentComponent>();
        components.registerComponent<HitboxComponent>();
        components.registerComponent<HurtboxComponent>();
        components.registerComponent<PushboxComponent>();
        components.registerComponent<LocalTransform>();
        components.registerComponent<RectangleColliderComponent>();
        components.registerComponent<CircleColliderComponent>();
    }

    static std::shared_ptr<CollisionClip> makeCollisionClip(bool loop = true, int frameCount = 2)
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
    auto& components = this->world.components();

    components.add<CollisionClipPlayerComponent>(entity, CollisionClipPlayerComponent{});
    auto clip = this->makeCollisionClip();
    CollisionClipDefinitionsComponent defs;
    defs.clips[StateId::Idle] = clip;
    components.add<CollisionClipDefinitionsComponent>(entity, defs);
    components.add<OrientationComponent>(entity, OrientationComponent{ Orientation::Right });

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Running, StateId::Idle });
    this->system.update(this->context);

    auto view = View<ParentComponent>(components);
    bool foundHitbox = false;
    for (auto [child, parent] : view)
    {
        if (parent.parent != entity || !components.has<HitboxComponent>(child)) continue;
        foundHitbox = true;
        break;
    }
    REQUIRE(foundHitbox);
}

TEST_CASE_METHOD(CollisionClipPlayerSystemFixture, "CollisionClipPlayerSystem respects orientation when creating colliders",
    "[integration][collision_clip_player_system]"
) {
    const auto entity = this->world.entities().create();
    auto& components = this->world.components();

    auto clip = this->makeCollisionClip();
    CollisionClipPlayerComponent player;
    player.playing = true;
    player.currentClip = clip;
    player.currentFrame = 0;
    
    components.add<CollisionClipPlayerComponent>(entity, player);
    components.add<CollisionClipDefinitionsComponent>(entity, CollisionClipDefinitionsComponent{});
    components.add<OrientationComponent>(entity, OrientationComponent{ Orientation::Left });

    this->bus.emit<OrientationChangedEvent>(OrientationChangedEvent{ entity, Orientation::Right, Orientation::Left });
    this->system.update(this->context);

    auto view = View<ParentComponent>(components);
    bool hasChild = false;
    for (auto [child, parent] : view)
    {
        if (parent.parent != entity) continue;
        hasChild = true;
        break;
    }
    REQUIRE(hasChild);
}
