add_library(game STATIC)

target_sources(game
    PRIVATE
        src/ComboLoader.cpp
        src/ComboSystem.cpp
        src/ConditionFactory.cpp
        src/GameScene.cpp
        src/InputBindingLoader.cpp
        src/RenderSystem.cpp
        src/StateMachineLoader.cpp
        src/StateMapper.cpp
        src/StateSystem.cpp
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
