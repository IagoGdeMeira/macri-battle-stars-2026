add_library(game STATIC)

target_sources(game
    PRIVATE
        src/collision_controllers/HitboxCollisionController.cpp
        src/collision_controllers/HurtboxCollisionController.cpp
        src/collision_controllers/PushboxCollisionController.cpp

        src/collision_detections/CircleCircleCollisionDetection.cpp
        src/collision_detections/RectCircleCollisionDetection.cpp
        src/collision_detections/RectRectCollisionDetection.cpp

        src/drawers/UIDrawer.cpp
        src/drawers/WorldDrawer.cpp

        src/flex_layout_handlers/AlignItemsHandler.cpp
        src/flex_layout_handlers/BoxModelHandler.cpp
        src/flex_layout_handlers/GrowHandler.cpp
        src/flex_layout_handlers/JustifyContentHandler.cpp
        src/flex_layout_handlers/MainAxisHandler.cpp

        src/render_formats/UICircleRenderFormat.cpp
        src/render_formats/UIFontRenderFormat.cpp
        src/render_formats/UIRectangleRenderFormat.cpp
        src/render_formats/UITextureRenderFormat.cpp
        src/render_formats/WorldCircleRenderFormat.cpp
        src/render_formats/WorldRectangleRenderFormat.cpp
        src/render_formats/WorldTextureRenderFormat.cpp

        src/scenes/GameScene.cpp
        src/scenes/HUDScene.cpp
        src/scenes/SelectionScene.cpp
        src/scenes/TitleScene.cpp

        src/ui_widget_loaders/HealthBarWidgetLoader.cpp
        src/ui_widget_loaders/TimerWidgetLoader.cpp

        src/utils/CollisionUtils.cpp
        src/utils/GameComponentRegistry.cpp
        src/utils/HUDComponentRegistry.cpp
        src/utils/WorldRenderUtils.cpp

        src/AirFrictionSystem.cpp
        src/AnimationLoader.cpp
        src/AnimationStateSystem.cpp
        src/AnimationSystem.cpp
        src/AttackSystem.cpp
        src/CameraControllerSystem.cpp
        src/CharacterDefinitionLoader.cpp
        src/CharacterLoader.cpp
        src/CharacterRoster.cpp
        src/CharacterRosterLoader.cpp
        src/CollisionControllerSystem.cpp
        src/CollisionDetectionSystem.cpp
        src/CollisionHandlerFactory.cpp
        src/ComboLoader.cpp
        src/ComboSystem.cpp
        src/DamageSystem.cpp
        src/DirectionTriggerSystem.cpp
        src/DynamicPushboxResolutionSystem.cpp
        src/EntityFactory.cpp
        src/FaceOffSystem.cpp
        src/FallTriggerSystem.cpp
        src/FlexEnumMapper.cpp
        src/FrictionSystem.cpp
        src/GravitySystem.cpp
        src/GroundDetectionSystem.cpp
        src/HealthBarSystem.cpp
        src/HitboxControllerSystem.cpp
        src/HitboxLoader.cpp
        src/HitstopSystem.cpp
        src/HoldTriggerSystem.cpp
        src/HorizontalMovementSystem.cpp
        src/HUDVisibilitySystem.cpp
        src/HurtboxControllerSystem.cpp
        src/HurtboxLoader.cpp
        src/JumpSystem.cpp
        src/KnockbackSystem.cpp
        src/LocalToWorldSystem.cpp
        src/MapLoader.cpp
        src/MapRoster.cpp
        src/MapRosterLoader.cpp
        src/MenuLoader.cpp
        src/MovementSystem.cpp
        src/PushboxControllerSystem.cpp
        src/PushboxLoader.cpp
        src/RoundTimerDisplaySystem.cpp
        src/RoundTimerSystem.cpp
        src/StateMachineLoader.cpp
        src/StateMachineRegistry.cpp
        src/StateIdMapper.cpp
        src/StateSystem.cpp
        src/StaticPushboxResolutionSystem.cpp
        src/TriggerBindingLoader.cpp
        src/TriggerConditionFactory.cpp
        src/TriggerGenerationSystem.cpp
        src/TriggerMapper.cpp
        src/UIActionFactory.cpp
        src/UIFactory.cpp
        src/UIFlexLayoutSystem.cpp
        src/UIInputSystem.cpp
        src/UILoader.cpp
        src/UIStyleLoader.cpp
)

target_include_directories(game
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/collision_controllers
        ${CMAKE_CURRENT_SOURCE_DIR}/collision_detections
        ${CMAKE_CURRENT_SOURCE_DIR}/collision_handlers
        ${CMAKE_CURRENT_SOURCE_DIR}/drawers
        ${CMAKE_CURRENT_SOURCE_DIR}/events
        ${CMAKE_CURRENT_SOURCE_DIR}/flex_layout_handlers
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/render_formats
        ${CMAKE_CURRENT_SOURCE_DIR}/scenes
        ${CMAKE_CURRENT_SOURCE_DIR}/trigger_conditions
        ${CMAKE_CURRENT_SOURCE_DIR}/ui_actions
        ${CMAKE_CURRENT_SOURCE_DIR}/ui_widget_loaders
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/value_objects
        ${CMAKE_CURRENT_SOURCE_DIR}/visual_effects
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(game
    PUBLIC
        engine
)

target_compile_features(game PUBLIC cxx_std_20)
