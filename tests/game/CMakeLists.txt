add_executable(game_tests
    trigger_conditions/CompositeConditionTests.cpp
    trigger_conditions/HealthBelowConditionTests.cpp
    trigger_conditions/InputPressedConditionTests.cpp
    trigger_conditions/MinTimeConditionTests.cpp
    trigger_conditions/NotConditionTests.cpp
    trigger_conditions/VelocityAboveConditionTests.cpp
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
    StateMachineLoaderTests.cpp
    StateMapperTests.cpp
    StateSystemTests.cpp
    StaticPushboxResolutionSystemTests.cpp
    TriggerBindingLoaderTests.cpp
    TriggerConditionFactoryTests.cpp
    TriggerGenerationSystemTests.cpp
    TriggerMapperTests.cpp
    UILayoutSystemTests.cpp
    WorldDrawerTests.cpp
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
