add_library(engine STATIC)

target_sources(engine
    PRIVATE
        src/CommandBuffer.cpp
        src/Engine.cpp
        src/MovementSystem.cpp
        src/Scene.cpp
        src/SceneManager.cpp
        src/SystemManager.cpp
)

target_include_directories(engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(engine
    PUBLIC
        domain
)

target_compile_features(engine PUBLIC cxx_std_20)
