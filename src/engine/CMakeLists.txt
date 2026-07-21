add_library(engine STATIC)

target_sources(engine
    PRIVATE
        src/CommandBuffer.cpp
        src/DataUtils.cpp
        src/Engine.cpp
        src/InputBindingLoader.cpp
        src/InputBufferSystem.cpp
        src/InputManager.cpp
        src/InputMapper.cpp
        src/InputSystem.cpp
        src/ResourceManager.cpp
        src/Scene.cpp
        src/SceneManager.cpp
)

target_include_directories(engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/events
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_SOURCE_DIR}/value_objects
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(engine
    PUBLIC
        domain
)

target_compile_features(engine PUBLIC cxx_std_20)
