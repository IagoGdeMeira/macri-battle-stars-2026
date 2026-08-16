add_executable(game_tests
    collision_controllers/HitboxCollisionControllerTests.cpp
    collision_controllers/HurtboxCollisionControllerTests.cpp
    collision_controllers/PushboxCollisionControllerTests.cpp

    collision_detections/CircleCircleCollisionDetectionTests.cpp
    collision_detections/RectCircleCollisionDetectionTests.cpp
    collision_detections/RectRectCollisionDetectionTests.cpp

    collision_handlers/CircleCollisionHandlerTests.cpp
    collision_handlers/RectangleCollisionHandlerTests.cpp

    drawers/UIDrawerTests.cpp
    drawers/WorldDrawerTests.cpp

    flex_layout_handlers/AlignItemsHandlerTests.cpp
    flex_layout_handlers/BoxModelHandlerTests.cpp
    flex_layout_handlers/GrowHandlerTests.cpp
    flex_layout_handlers/JustifyContentHandlerTests.cpp
    flex_layout_handlers/MainAxisHandlerTests.cpp

    render_formats/UICircleRenderFormatTests.cpp
    render_formats/UIFontRenderFormatTests.cpp
    render_formats/UIRectangleRenderFormatTests.cpp
    render_formats/UITextureRenderFormatTests.cpp
    render_formats/WorldCircleRenderFormatTests.cpp
    render_formats/WorldRectangleRenderFormatTests.cpp
    render_formats/WorldTextureRenderFormatTests.cpp

    scenes/GameSceneTests.cpp

    trigger_conditions/CompositeConditionTests.cpp
    trigger_conditions/HealthBelowConditionTests.cpp
    trigger_conditions/InputPressedConditionTests.cpp
    trigger_conditions/MinTimeConditionTests.cpp
    trigger_conditions/NotConditionTests.cpp
    trigger_conditions/VelocityAboveConditionTests.cpp
    trigger_conditions/VerticalVelocityAboveConditionTests.cpp

    ui_actions/NavigateToSelectionActionTests.cpp
    ui_actions/NavigateToTitleActionTests.cpp
    ui_actions/QuitActionTests.cpp

    utils/CollisionUtilsTests.cpp
    utils/GameComponentRegistryTests.cpp
    utils/HUDComponentRegistryTests.cpp
    utils/WorldRenderUtilsTests.cpp

    value_objects/ComboTests.cpp

    visual_effects/OutlineFontEffectTests.cpp
    visual_effects/OutlineTextureEffectTests.cpp
    visual_effects/ShadowFontEffectTests.cpp
    visual_effects/ShadowTextureEffectTests.cpp

    AirFrictionSystemTests.cpp
    AnimationLoaderTests.cpp
    AnimationStateSystemTests.cpp
    AnimationSystemTests.cpp
    AttackSystemTests.cpp
    CameraControllerSystemTests.cpp
    CharacterDefinitionLoaderTests.cpp
    CharacterLoaderTests.cpp
    CharacterRosterTests.cpp
    CharacterRosterLoaderTests.cpp
    CollisionControllerSystemTests.cpp
    CollisionDetectionSystemTests.cpp
    CollisionHandlerFactoryTests.cpp
    ComboLoaderTests.cpp
    ComboSystemTests.cpp
    DirectionTriggerSystemTests.cpp
    DynamicPushboxResolutionSystemTests.cpp
    EntityFactoryTests.cpp
    FaceOffSystemTests.cpp
    FallTriggerSystemTests.cpp
    FrictionSystemTests.cpp
    GravitySystemTests.cpp
    GroundDetectionSystemTests.cpp
    HealthBarSystemTests.cpp
    HitboxControllerSystemTests.cpp
    HitboxLoaderTests.cpp
    HitstopSystemTests.cpp
    HoldTriggerSystemTests.cpp
    HorizontalMovementSystemTests.cpp
    HUDVisibilitySystemTests.cpp
    HurtboxControllerSystemTests.cpp
    HurtboxLoaderTests.cpp
    JumpSystemTests.cpp
    KnockbackSystemTests.cpp
    LocalToWorldSystemTests.cpp
    MenuLoaderTests.cpp
    MapLoaderTests.cpp
    MapRosterTests.cpp
    MapRosterLoaderTests.cpp
    MenuLoaderTests.cpp
    MovementSystemTests.cpp
    PushboxControllerSystemTests.cpp
    PushboxLoaderTests.cpp
    RoundTimerDisplaySystemTests.cpp
    RoundTimerSystemTests.cpp
    StateMachineLoaderTests.cpp
    StateSystemTests.cpp
    StaticPushboxResolutionSystemTests.cpp
    TriggerBindingLoaderTests.cpp
    TriggerConditionFactoryTests.cpp
    TriggerGenerationSystemTests.cpp
    TriggerMapperTests.cpp
    UIFactoryTests.cpp
    UIActionFactoryTests.cpp
    UIInputSystemTests.cpp
)

target_link_libraries(game_tests
    PRIVATE
        Catch2::Catch2WithMain
        game
        tests_common
)

if(MSVC)
    target_compile_options(game_tests PRIVATE /bigobj)
endif()

add_test(NAME game_tests COMMAND game_tests)
