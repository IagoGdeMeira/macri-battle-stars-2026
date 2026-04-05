add_executable(game_tests
    ComboLoaderTests.cpp
    ComboSystemTests.cpp
    ComboTests.cpp
    ConditionFactoryTests.cpp
    GameSceneTests.cpp
    InputBindingLoaderTests.cpp
    StateMachineLoaderTests.cpp
    StateMapperTests.cpp
    StateSystemTests.cpp
    TriggerMapperTests.cpp
)

target_link_libraries(game_tests
    PRIVATE
        game
        Catch2::Catch2WithMain
)

add_test(NAME game_tests COMMAND game_tests)
