#include "../../src/game/include/GameScene/GameScene.h"

#include "../../src/domain/components/InputBufferComponent.h"
#include "../../src/domain/components/InputComponent.h"
#include "../../src/domain/components/PlayerComponent.h"
#include "../../src/domain/components/StateComponent.h"
#include "../../src/domain/include/View/View.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/InputBinding/InputBinding.h"

#include <catch2/catch_test_macros.hpp>
#include <utility>

TEST_CASE("GameScene init registers core gameplay components",
    "[integration][game_scene]"
) {
    EventBus bus;
    InputContext input;
    input.bindings.emplace(7, InputBinding{});

    std::vector<Combo> combos;
    StateMachine machine;

    GameScene scene(bus, input, combos, std::move(machine));

    scene.init();

    View<PlayerComponent,
        InputComponent,
        InputBufferComponent,
        StateComponent
    > view(scene.world().components());

    size_t count = 0;
    for (auto [entity, player, inputComponent, buffer, state] : view)
    {
        ++count;
        REQUIRE(player.id == 7);
        REQUIRE(inputComponent.actions.empty());
        REQUIRE(buffer.buffer.empty());
        REQUIRE(state.current == StateId::Idle);
        REQUIRE(state.timeInState == 0.0f);
    }

    REQUIRE(count == 1);
}
