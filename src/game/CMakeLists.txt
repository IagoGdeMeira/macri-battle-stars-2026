add_library(game STATIC)

target_sources(game
    PRIVATE
        src/scenes/GameScene.cpp
        src/scenes/SelectionScene.cpp
        src/scenes/TitleScene.cpp
        src/AnimationLoader.cpp
        src/AnimationStateSystem.cpp
        src/AnimationSystem.cpp
        src/Camera2D.cpp
        src/CameraControllerSystem.cpp
        src/CharacterDefinitionLoader.cpp
        src/CharacterLoader.cpp
        src/CharacterRoster.cpp
        src/CharacterRosterLoader.cpp
        src/CollisionDetectionSystem.cpp
        src/ComboLoader.cpp
        src/ComboSystem.cpp
        src/ComponentRegistry.cpp
        src/ConditionFactory.cpp
        src/DamageSystem.cpp
        src/FrictionSystem.cpp
        src/GravitySystem.cpp
        src/LocalToWorldSystem.cpp
        src/MapLoader.cpp
        src/MapRoster.cpp
        src/MapRosterLoader.cpp
        src/RenderSystem.cpp
        src/StateMachineLoader.cpp
        src/StateMapper.cpp
        src/StateSystem.cpp
        src/TriggerBindingLoader.cpp
        src/TriggerGenerationSystem.cpp
        src/TriggerMapper.cpp
)

target_include_directories(game
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(game
    PUBLIC
        engine
        domain
)

target_compile_features(game PUBLIC cxx_std_20)
