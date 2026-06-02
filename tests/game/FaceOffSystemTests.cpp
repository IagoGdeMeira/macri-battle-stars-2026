#include "../src/game/include/FaceOffSystem/FaceOffSystem.h"

#include "../src/domain/components/OrientationComponent.h"
#include "../src/domain/components/PlayerComponent.h"
#include "../src/domain/components/TransformComponent.h"
#include "../src/domain/events/OrientationChangedEvent.h"

#include "../src/engine/include/EventBus/EventBus.h"
#include "../src/engine/include/Scene/Scene.h"

#include <catch2/catch_test_macros.hpp>
#include <vector>

class FaceOffSystemFixture
{
public:
    FaceOffSystemFixture()
    {
        auto& components = this->scene.world().components();
        components.registerComponent<TransformComponent>();
        components.registerComponent<OrientationComponent>();
        components.registerComponent<PlayerComponent>();
    }

    EventBus bus;
    Scene scene{bus};
};

TEST_CASE_METHOD(FaceOffSystemFixture, "FaceOffSystem with no players does nothing",
    "[unit][face_off_system]"
) {
    this->scene.systems().addSystem<FaceOffSystem>(this->bus);
    
    std::vector<OrientationChangedEvent> events;
    this->bus.subscribe<OrientationChangedEvent>([&](const OrientationChangedEvent& event)
    { events.push_back(event); });

    this->scene.update(0.016f);

    REQUIRE(events.empty());
}

TEST_CASE_METHOD(FaceOffSystemFixture, "FaceOffSystem with one player does nothing",
    "[unit][face_off_system]"
) {
    const auto entity = this->scene.world().entities().create();
    
    auto& components = this->scene.world().components();
    components.add<TransformComponent>(entity, TransformComponent{0.0f, 0.0f});
    components.add<OrientationComponent>(entity, OrientationComponent{Orientation::Right});
    components.add<PlayerComponent>(entity, PlayerComponent{1});

    this->scene.systems().addSystem<FaceOffSystem>(this->bus);
    
    std::vector<OrientationChangedEvent> events;
    this->bus.subscribe<OrientationChangedEvent>([&](const OrientationChangedEvent& event)
    { events.push_back(event); });

    this->scene.update(0.016f);

    REQUIRE(events.empty());
}

TEST_CASE_METHOD(FaceOffSystemFixture, "FaceOffSystem sets correct orientations for two players",
    "[integration][face_off_system]"
) {
    auto& entities = this->scene.world().entities();
    const auto player1 = entities.create();
    const auto player2 = entities.create();

    auto& components = this->scene.world().components();
    components.add<TransformComponent>(player1, TransformComponent{50.0f, 0.0f});
    components.add<OrientationComponent>(player1, OrientationComponent{Orientation::Left});
    components.add<PlayerComponent>(player1, PlayerComponent{1});

    components.add<TransformComponent>(player2, TransformComponent{100.0f, 0.0f});
    components.add<OrientationComponent>(player2, OrientationComponent{Orientation::Right});
    components.add<PlayerComponent>(player2, PlayerComponent{2});

    std::vector<OrientationChangedEvent> events;
    this->bus.subscribe<OrientationChangedEvent>([&](const OrientationChangedEvent& event)
    { events.push_back(event); });

    this->scene.systems().addSystem<FaceOffSystem>(this->bus);
    this->scene.update(0.016f);

    REQUIRE(events.size() == 2);
    
    const auto& orientation1 = components.get<OrientationComponent>(player1);
    const auto& orientation2 = components.get<OrientationComponent>(player2);
    
    REQUIRE(orientation1.direction == Orientation::Right);
    REQUIRE(orientation2.direction == Orientation::Left);
}

TEST_CASE_METHOD(FaceOffSystemFixture, "FaceOffSystem emits events when orientations change",
    "[integration][face_off_system]"
) {
    auto& entities = this->scene.world().entities();
    const auto player1 = entities.create();
    const auto player2 = entities.create();

    auto& components = this->scene.world().components();
    components.add<TransformComponent>(player1, TransformComponent{50.0f, 0.0f});
    components.add<OrientationComponent>(player1, OrientationComponent{Orientation::Left});
    components.add<PlayerComponent>(player1, PlayerComponent{1});

    components.add<TransformComponent>(player2, TransformComponent{100.0f, 0.0f});
    components.add<OrientationComponent>(player2, OrientationComponent{Orientation::Right});
    components.add<PlayerComponent>(player2, PlayerComponent{2});

    std::vector<OrientationChangedEvent> events;
    this->bus.subscribe<OrientationChangedEvent>([&](const OrientationChangedEvent& event)
    { events.push_back(event); });

    this->scene.systems().addSystem<FaceOffSystem>(this->bus);
    this->scene.update(0.016f);

    REQUIRE(events.size() == 2);
    REQUIRE(events[0].entity == player1);
    REQUIRE(events[0].previous == Orientation::Left);
    REQUIRE(events[0].current == Orientation::Right);
    
    REQUIRE(events[1].entity == player2);
    REQUIRE(events[1].previous == Orientation::Right);
    REQUIRE(events[1].current == Orientation::Left);
}

TEST_CASE_METHOD(FaceOffSystemFixture, "FaceOffSystem does not emit events when orientations don't change",
    "[integration][face_off_system]"
) {
    auto& entities = this->scene.world().entities();
    const auto player1 = entities.create();
    const auto player2 = entities.create();
    
    auto& components = this->scene.world().components();
    components.add<TransformComponent>(player1, TransformComponent{50.0f, 0.0f});
    components.add<OrientationComponent>(player1, OrientationComponent{Orientation::Right});
    components.add<PlayerComponent>(player1, PlayerComponent{1});

    components.add<TransformComponent>(player2, TransformComponent{100.0f, 0.0f});
    components.add<OrientationComponent>(player2, OrientationComponent{Orientation::Left});
    components.add<PlayerComponent>(player2, PlayerComponent{2});

    this->scene.systems().addSystem<FaceOffSystem>(this->bus);
    
    std::vector<OrientationChangedEvent> events;
    this->bus.subscribe<OrientationChangedEvent>([&](const OrientationChangedEvent& event)
    { events.push_back(event); });

    this->scene.update(0.016f);
    REQUIRE(events.size() == 0);

    this->scene.update(0.016f);
    REQUIRE(events.size() == 0);
}

TEST_CASE_METHOD(FaceOffSystemFixture, "FaceOffSystem correctly orders and orients three players",
    "[integration][face_off_system]"
) {
    auto& entities = this->scene.world().entities();
    const auto player1 = entities.create();
    const auto player2 = entities.create();
    const auto player3 = entities.create();

    auto& components = this->scene.world().components();
    components.add<TransformComponent>(player1, TransformComponent{200.0f, 0.0f});
    components.add<OrientationComponent>(player1, OrientationComponent{Orientation::Right});
    components.add<PlayerComponent>(player1, PlayerComponent{1});

    components.add<TransformComponent>(player2, TransformComponent{50.0f, 0.0f});
    components.add<OrientationComponent>(player2, OrientationComponent{Orientation::Left});
    components.add<PlayerComponent>(player2, PlayerComponent{2});

    components.add<TransformComponent>(player3, TransformComponent{125.0f, 0.0f});
    components.add<OrientationComponent>(player3, OrientationComponent{Orientation::Right});
    components.add<PlayerComponent>(player3, PlayerComponent{3});

    std::vector<OrientationChangedEvent> events;
    this->bus.subscribe<OrientationChangedEvent>([&](const OrientationChangedEvent& event)
    { events.push_back(event); });

    this->scene.systems().addSystem<FaceOffSystem>(this->bus);
    this->scene.update(0.016f);

    REQUIRE(events.size() == 3);

    const auto& orientation2 = components.get<OrientationComponent>(player2);
    REQUIRE(orientation2.direction == Orientation::Right);

    const auto& orientation3 = components.get<OrientationComponent>(player3);
    REQUIRE(orientation3.direction == Orientation::Left);

    const auto& orientation1 = components.get<OrientationComponent>(player1);
    REQUIRE(orientation1.direction == Orientation::Left);
}
