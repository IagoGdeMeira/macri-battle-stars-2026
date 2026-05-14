add_executable(game_tests
    conditions/CompositeConditionTests.cpp
    conditions/HealthBelowConditionTests.cpp
    conditions/MinTimeConditionTests.cpp
    conditions/NotConditionTests.cpp
    conditions/VelocityAboveConditionTests.cpp
    AirFrictionSystemTests.cpp
    AnimationLoaderTests.cpp
    AnimationStateSystemTests.cpp
    AnimationSystemTests.cpp
    Camera2DTests.cpp    CameraControllerSystemTests.cpp
    ComponentRegistryTests.cpp
    CharacterDefinitionLoaderTests.cpp
    CharacterLoaderTests.cpp
    CollisionClipLoaderTests.cpp
    CollisionClipPlayerSystemTests.cpp
    CollisionDetectionSystemTests.cpp
    CollisionHandlerFactoryTests.cpp
    ComboLoaderTests.cpp
    ComboSystemTests.cpp
    ComboTests.cpp
    ConditionFactoryTests.cpp
    DynamicPushboxResolutionSystemTests.cpp
    FaceOffSystemTests.cpp
    FrictionSystemTests.cpp
    GameSceneTests.cpp
    GravitySystemTests.cpp
    GroundDetectionSystemTests.cpp
    HitstopSystemTests.cpp
    KnockbackSystemTests.cpp
    LocalToWorldSystemTests.cpp
    PlayerControlSystemTests.cpp
    RenderSystemTests.cpp
    StateMachineLoaderTests.cpp
    StateMapperTests.cpp
    StateSystemTests.cpp
    StaticPushboxResolutionSystemTests.cpp
    TriggerBindingLoaderTests.cpp
    TriggerGenerationSystemTests.cpp
    TriggerMapperTests.cpp
    UILayoutSystemTests.cpp
)

target_link_libraries(game_tests
    PRIVATE
        game
        Catch2::Catch2WithMain
)

if(MSVC)
    target_compile_options(game_tests PRIVATE /bigobj)
endif()

add_test(NAME game_tests COMMAND game_tests)
