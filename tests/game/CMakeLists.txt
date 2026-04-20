add_executable(game_tests
    AnimationLoaderTests.cpp
    AnimationStateSystemTests.cpp
    AnimationSystemTests.cpp
    Camera2DTests.cpp
    CameraControllerSystemTests.cpp
    ComponentRegistryTests.cpp
    CharacterDefinitionLoaderTests.cpp
    CharacterLoaderTests.cpp
    CollisionDetectionSystemTests.cpp
    ComboLoaderTests.cpp
    ComboSystemTests.cpp
    ComboTests.cpp
    ConditionFactoryTests.cpp
    conditions/CompositeConditionTests.cpp
    conditions/HealthBelowConditionTests.cpp
    conditions/MinTimeConditionTests.cpp
    conditions/NotConditionTests.cpp
    conditions/VelocityAboveConditionTests.cpp
    GameSceneTests.cpp
    LocalToWorldSystemTests.cpp
    RenderSystemTests.cpp
    StateMachineLoaderTests.cpp
    StateMapperTests.cpp
    StateSystemTests.cpp
    TextureLoaderTests.cpp
    TriggerBindingLoaderTests.cpp
    TriggerGenerationSystemTests.cpp
    TriggerMapperTests.cpp
)

target_link_libraries(game_tests
    PRIVATE
        game
        Catch2::Catch2WithMain
)

add_test(NAME game_tests COMMAND game_tests)
