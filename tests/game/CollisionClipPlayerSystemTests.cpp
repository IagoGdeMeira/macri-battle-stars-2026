#include "../../src/game/include/CollisionClipPlayerSystem/CollisionClipPlayerSystem.h"

#include "../../src/domain/components/CollisionClipPlayerComponent.h"
#include "../../src/domain/components/CollisionClipDefinitionsComponent.h"
#include "../../src/domain/components/OrientationComponent.h"
#include "../../src/domain/include/CollisionFrame/CollisionFrame.h"
#include "../../src/domain/include/World/World.h"
#include "../../src/domain/include/View/View.h"
#include "../../src/domain/events/OrientationChangedEvent.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"
#include "../../src/engine/include/System/System.h"

#include "../../src/game/events/StateChangedEvent.h"

#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <memory>
#include <vector>
#include <unordered_map>

class CollisionClipPlayerSystemFixture
{
public:
    class MockEntityFactory
    {
    public:
        struct CreatedEntity
        {
            Entity parent;
            std::string type;
            bool facingLeft;
        };

        explicit MockEntityFactory(World& world) : world(world) {}

        std::vector<CreatedEntity> createdEntities;

        Entity createHitbox(Entity parent, const HitboxDef& def, bool facingLeft)
        {
            (void)def;
            auto child = this->world.entities().create();
            this->createdEntities.push_back({ parent, "hitbox", facingLeft });
            return child;
        }

        Entity createHurtbox(Entity parent, const HurtboxDef& def, bool facingLeft)
        {
            (void)def;
            auto child = this->world.entities().create();
            this->createdEntities.push_back({ parent, "hurtbox", facingLeft });
            return child;
        }

        Entity createPushbox(Entity parent, const PushboxDef& def, bool facingLeft)
        {
            (void)def;
            auto child = this->world.entities().create();
            this->createdEntities.push_back({ parent, "pushbox", facingLeft });
            return child;
        }

    private:
        World& world;
    };

    class TestableCollisionClipPlayerSystem : public System
    {
    public:
        TestableCollisionClipPlayerSystem(EventBus& bus, MockEntityFactory& factory)
            : bus(bus), factory(factory)
        {
            bus.subscribe<StateChangedEvent>([this](const StateChangedEvent& e)
            { this->stateChanges.push_back(e); });

            bus.subscribe<OrientationChangedEvent>([this](const OrientationChangedEvent& e)
            { this->orientationChanges.push_back(e); });
        }

        void update(UpdateContext& ctx) override
        {
            auto& components = ctx.world.components();

            for (const auto& sc : this->stateChanges)
            {
                if (!components.has<CollisionClipPlayerComponent>(sc.entity)) continue;
                if (!components.has<CollisionClipDefinitionsComponent>(sc.entity)) continue;

                auto& player = components.get<CollisionClipPlayerComponent>(sc.entity);
                auto& defs = components.get<CollisionClipDefinitionsComponent>(sc.entity);

                auto it = defs.clips.find(sc.current);
                if (it != defs.clips.end() && it->second)
                {
                    player.currentClip = it->second;
                    player.currentFrame = 0;
                    player.elapsedTime = 0.0f;
                    player.playing = true;
                }
                else player.playing = false;

                this->refreshColliders(ctx, sc.entity, player);
            }
            this->stateChanges.clear();

            auto view = View<CollisionClipPlayerComponent>(components);
            for (auto [entity, player] : view)
            {
                if (!player.playing || !player.currentClip) continue;
                if (player.currentClip->frames.empty()) continue;

                player.elapsedTime += ctx.deltaTime;
                const auto& frame = player.currentClip->frames[player.currentFrame];

                if (player.elapsedTime < frame.duration) continue;

                player.elapsedTime -= frame.duration;
                player.currentFrame++;

                if (player.currentFrame >= (int)player.currentClip->frames.size())
                {
                    if (player.currentClip->loop) player.currentFrame = 0;
                    else
                    {
                        player.playing = false;
                        player.currentFrame = static_cast<int>(player.currentClip->frames.size()) - 1;
                    }
                }
                this->refreshColliders(ctx, entity, player);
            }

            for (const auto& oc : this->orientationChanges)
            {
                if (!components.has<CollisionClipPlayerComponent>(oc.entity)) continue;
                if (!components.has<CollisionClipDefinitionsComponent>(oc.entity)) continue;

                auto& player = components.get<CollisionClipPlayerComponent>(oc.entity);
                if (player.playing) this->refreshColliders(ctx, oc.entity, player);
            }
            this->orientationChanges.clear();
        }

    private:
        EventBus& bus;
        MockEntityFactory& factory;

        std::unordered_map<Entity, std::vector<Entity>, Entity::Hash> activeColliders;
        std::vector<StateChangedEvent> stateChanges;
        std::vector<OrientationChangedEvent> orientationChanges;

        void refreshColliders(UpdateContext& ctx, Entity owner, CollisionClipPlayerComponent& player)
        {
            auto& comp = ctx.world.components();

            auto it = this->activeColliders.find(owner);
            if (it != this->activeColliders.end())
            {
                for (Entity child : it->second) ctx.world.entities().destroy(child);
                it->second.clear();
            }
            if (!player.playing || !player.currentClip) return;

            bool facingLeft = comp.has<OrientationComponent>(owner)
                ? (comp.get<OrientationComponent>(owner).direction == Orientation::Left)
                : false;

            const auto& frame = player.currentClip->frames[player.currentFrame];
            std::vector<Entity> newChildren;

            for (const auto& hb : frame.hitboxes)
            { newChildren.push_back(this->factory.createHitbox(owner, hb, facingLeft)); }

            for (const auto& hb : frame.hurtboxes)
            { newChildren.push_back(this->factory.createHurtbox(owner, hb, facingLeft)); }

            for (const auto& pb : frame.pushboxes)
            { newChildren.push_back(this->factory.createPushbox(owner, pb, facingLeft)); }

            this->activeColliders[owner] = std::move(newChildren);
        }
    };

    CollisionClipPlayerSystemFixture()
        : factory(world), system(bus, factory), context{ world, bus, commandBuffer, 0.016f }
    {
        auto& components = this->world.components();
        components.registerComponent<CollisionClipPlayerComponent>();
        components.registerComponent<CollisionClipDefinitionsComponent>();
        components.registerComponent<OrientationComponent>();
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
                dynamic_cast<CircleDef*>(hb.collider.get())->radius = 5.0f;
                frame.hitboxes.push_back(std::move(hb));
            }

            clip->frames.push_back(std::move(frame));
        }

        return clip;
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    MockEntityFactory factory;
    TestableCollisionClipPlayerSystem system;
    UpdateContext context;
};

TEST_CASE_METHOD(CollisionClipPlayerSystemFixture,
    "CollisionClipPlayerSystem starts clip when state changes",
    "[integration][collision_clip_player_system]"
) {
    const auto entity = this->world.entities().create();

    auto& components = this->world.components();

    CollisionClipPlayerComponent player;
    player.playing = false;
    components.add<CollisionClipPlayerComponent>(entity, player);

    auto clip = makeCollisionClip();
    CollisionClipDefinitionsComponent defs;
    defs.clips[StateId::Idle] = clip;
    components.add<CollisionClipDefinitionsComponent>(entity, defs);

    this->context.deltaTime = 0.0f;
    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Running, StateId::Idle });
    this->system.update(this->context);

    const auto& updated = components.get<CollisionClipPlayerComponent>(entity);
    REQUIRE(updated.playing == true);
    REQUIRE(updated.currentClip == clip);
    REQUIRE(updated.currentFrame == 0);
    REQUIRE(updated.elapsedTime == 0.0f);
}

TEST_CASE_METHOD(CollisionClipPlayerSystemFixture,
    "CollisionClipPlayerSystem stops clip when state not found",
    "[integration][collision_clip_player_system]"
) {
    const auto entity = this->world.entities().create();

    auto& components = this->world.components();

    CollisionClipPlayerComponent player;
    player.playing = true;
    player.currentClip = makeCollisionClip();
    components.add<CollisionClipPlayerComponent>(entity, player);

    CollisionClipDefinitionsComponent defs;
    components.add<CollisionClipDefinitionsComponent>(entity, defs);

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Idle, StateId::Running });
    this->system.update(this->context);

    const auto& updated = components.get<CollisionClipPlayerComponent>(entity);
    REQUIRE(updated.playing == false);
}

TEST_CASE_METHOD(CollisionClipPlayerSystemFixture,
    "CollisionClipPlayerSystem advances frame when elapsed time exceeds duration",
    "[integration][collision_clip_player_system]"
) {
    const auto entity = this->world.entities().create();

    auto& components = this->world.components();

    auto clip = makeCollisionClip(true, 3);
    CollisionClipPlayerComponent player;
    player.playing = true;
    player.currentClip = clip;
    player.currentFrame = 0;
    player.elapsedTime = 0.08f;
    components.add<CollisionClipPlayerComponent>(entity, player);

    components.add<CollisionClipDefinitionsComponent>(entity, CollisionClipDefinitionsComponent{});

    this->context.deltaTime = 0.05f;
    this->system.update(this->context);

    const auto& updated = components.get<CollisionClipPlayerComponent>(entity);
    REQUIRE(updated.currentFrame == 1);
    REQUIRE(std::fabs(updated.elapsedTime - 0.03f) < 0.0001f);
}

TEST_CASE_METHOD(CollisionClipPlayerSystemFixture,
    "CollisionClipPlayerSystem loops to first frame when reaching end",
    "[integration][collision_clip_player_system]"
) {
    const auto entity = this->world.entities().create();

    auto& components = this->world.components();

    auto clip = makeCollisionClip(true, 2);
    CollisionClipPlayerComponent player;
    player.playing = true;
    player.currentClip = clip;
    player.currentFrame = 1;
    player.elapsedTime = 0.08f;
    components.add<CollisionClipPlayerComponent>(entity, player);

    components.add<CollisionClipDefinitionsComponent>(entity, CollisionClipDefinitionsComponent{});

    this->context.deltaTime = 0.05f;
    this->system.update(this->context);

    const auto& updated = components.get<CollisionClipPlayerComponent>(entity);
    REQUIRE(updated.currentFrame == 0);
    REQUIRE(updated.playing == true);
}

TEST_CASE_METHOD(CollisionClipPlayerSystemFixture,
    "CollisionClipPlayerSystem stops at last frame when loop disabled",
    "[integration][collision_clip_player_system]"
) {
    const auto entity = this->world.entities().create();

    auto& components = this->world.components();

    auto clip = makeCollisionClip(false, 2);
    CollisionClipPlayerComponent player;
    player.playing = true;
    player.currentClip = clip;
    player.currentFrame = 1;
    player.elapsedTime = 0.08f;
    components.add<CollisionClipPlayerComponent>(entity, player);

    components.add<CollisionClipDefinitionsComponent>(entity, CollisionClipDefinitionsComponent{});

    this->context.deltaTime = 0.05f;
    this->system.update(this->context);

    const auto& updated = components.get<CollisionClipPlayerComponent>(entity);
    REQUIRE(updated.currentFrame == 1);
    REQUIRE(updated.playing == false);
}

TEST_CASE_METHOD(CollisionClipPlayerSystemFixture,
    "CollisionClipPlayerSystem ignores state change for entities without required components",
    "[integration][collision_clip_player_system]"
) {
    const auto entity = this->world.entities().create();

    auto& components = this->world.components();

    CollisionClipPlayerComponent player;
    player.playing = false;
    components.add<CollisionClipPlayerComponent>(entity, player);

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Running, StateId::Idle });
    this->system.update(this->context);

    const auto& unchanged = components.get<CollisionClipPlayerComponent>(entity);
    REQUIRE(unchanged.playing == false);
}

TEST_CASE_METHOD(CollisionClipPlayerSystemFixture,
    "CollisionClipPlayerSystem updates colliders on state change",
    "[integration][collision_clip_player_system]"
) {
    const auto entity = this->world.entities().create();

    auto& components = this->world.components();

    auto clip = makeCollisionClip();
    CollisionClipPlayerComponent player;
    player.playing = false;
    components.add<CollisionClipPlayerComponent>(entity, player);

    CollisionClipDefinitionsComponent defs;
    defs.clips[StateId::Idle] = clip;
    components.add<CollisionClipDefinitionsComponent>(entity, defs);

    components.add<OrientationComponent>(entity, OrientationComponent{ Orientation::Right });

    factory.createdEntities.clear();

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Running, StateId::Idle });
    this->system.update(this->context);

    REQUIRE(factory.createdEntities.size() > 0);
    for (const auto& created : factory.createdEntities)
    {
        REQUIRE(created.parent == entity);
        REQUIRE(created.facingLeft == false);
    }
}

TEST_CASE_METHOD(CollisionClipPlayerSystemFixture,
    "CollisionClipPlayerSystem respects orientation when creating colliders",
    "[integration][collision_clip_player_system]"
) {
    const auto entity = this->world.entities().create();

    auto& components = this->world.components();

    auto clip = makeCollisionClip();
    CollisionClipPlayerComponent player;
    player.playing = true;
    player.currentClip = clip;
    player.currentFrame = 0;
    components.add<CollisionClipPlayerComponent>(entity, player);

    components.add<CollisionClipDefinitionsComponent>(entity, CollisionClipDefinitionsComponent{});

    components.add<OrientationComponent>(entity, OrientationComponent{ Orientation::Left });

    factory.createdEntities.clear();

    this->bus.emit<OrientationChangedEvent>(
        OrientationChangedEvent{ entity, Orientation::Right, Orientation::Left });
    this->system.update(this->context);

    REQUIRE(factory.createdEntities.size() > 0);
    for (const auto& created : factory.createdEntities)
    {
        REQUIRE(created.facingLeft == true);
    }
}

TEST_CASE_METHOD(CollisionClipPlayerSystemFixture,
    "CollisionClipPlayerSystem destroys previous colliders before creating new ones",
    "[integration][collision_clip_player_system]"
) {
    const auto entity = this->world.entities().create();

    auto& components = this->world.components();

    auto clip = makeCollisionClip();
    CollisionClipPlayerComponent player;
    player.playing = true;
    player.currentClip = clip;
    player.currentFrame = 0;
    components.add<CollisionClipPlayerComponent>(entity, player);

    components.add<CollisionClipDefinitionsComponent>(entity, CollisionClipDefinitionsComponent{});
    components.add<OrientationComponent>(entity, OrientationComponent{ Orientation::Right });

    this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, StateId::Running, StateId::Idle });
    this->system.update(this->context);

    const auto initialCount = factory.createdEntities.size();

    this->bus.emit<OrientationChangedEvent>(
        OrientationChangedEvent{ entity, Orientation::Right, Orientation::Left });
    this->system.update(this->context);

    const auto secondUpdateCount = factory.createdEntities.size();
    REQUIRE(secondUpdateCount == initialCount * 2);
}

TEST_CASE_METHOD(CollisionClipPlayerSystemFixture,
    "CollisionClipPlayerSystem ignores orientation change if not playing",
    "[integration][collision_clip_player_system]"
) {
    const auto entity = this->world.entities().create();

    auto& components = this->world.components();

    CollisionClipPlayerComponent player;
    player.playing = false;
    components.add<CollisionClipPlayerComponent>(entity, player);

    components.add<CollisionClipDefinitionsComponent>(entity, CollisionClipDefinitionsComponent{});
    components.add<OrientationComponent>(entity, OrientationComponent{ Orientation::Right });

    factory.createdEntities.clear();

    this->bus.emit<OrientationChangedEvent>(
        OrientationChangedEvent{ entity, Orientation::Right, Orientation::Left });
    this->system.update(this->context);

    REQUIRE(factory.createdEntities.empty());
}
