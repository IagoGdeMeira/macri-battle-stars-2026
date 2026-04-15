#include "../../src/game/include/GameScene/GameScene.h"

#include "../../src/domain/components/InputBufferComponent.h"
#include "../../src/domain/components/InputComponent.h"
#include "../../src/domain/components/PlayerComponent.h"
#include "../../src/domain/components/StateComponent.h"
#include "../../src/domain/components/StateMachineComponent.h"
#include "../../src/domain/include/View/View.h"

#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/InputBinding/InputBinding.h"
#include "../../src/engine/include/Window/Window.h"

#include "../../src/game/include/Camera2D/Camera2D.h"

#include <catch2/catch_test_macros.hpp>
#include <utility>

class GameSceneFixture
{
public:
    class StubWindow : public Window
    {
    public:
        void create(int w, int h, const char* /*title*/) override
        {
            this->width = w;
            this->height = h;
        }

        void setResolution(int w, int h) override
        {
            this->width = w;
            this->height = h;
        }

        void setFullscreen(bool enabled) override
        {
            this->fullscreen = enabled;
        }

        void getSize(int& w, int& h) override
        {
            w = this->width;
            h = this->height;
        }

    private:
        int width = 800;
        int height = 600;
        bool fullscreen = false;
    };
};

TEST_CASE_METHOD(GameSceneFixture, "GameScene init registers core gameplay components",
    "[integration][game_scene]"
) {
    EventBus bus;
    InputContext input;
    input.bindings.emplace(7, InputBinding{});
    TriggerContext triggerContext;

    std::vector<Combo> combos;
    StateMachine machine;
    Camera2D camera;
    StubWindow window;

    GameScene scene(
        bus,
        input,
        std::move(triggerContext),
        combos,
        std::move(machine),
        camera,
        window);

    scene.init();

    View<PlayerComponent,
        InputComponent,
        InputBufferComponent,
        StateComponent,
        StateMachineComponent
    > view(scene.world().components());

    size_t count = 0;
    for (auto [entity, player, inputComponent, buffer, state, stateMachine] : view)
    {
        ++count;
        (void)entity;
        (void)stateMachine;

        REQUIRE(player.id == 7);
        REQUIRE(inputComponent.actions.empty());
        REQUIRE(buffer.buffer.empty());
        REQUIRE(state.current == StateId::Idle);
        REQUIRE(state.timeInState == 0.0f);
    }

    REQUIRE(count == 1);
}
