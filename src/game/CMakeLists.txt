add_library(game STATIC)

target_sources(game
    PRIVATE
        src/systems/DamageSystem.cpp
        src/systems/MovementSystem.cpp
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
