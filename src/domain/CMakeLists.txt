add_library(domain STATIC)

target_sources(domain
    PRIVATE
        src/ComponentManager.cpp
        src/EntityManager.cpp
        src/EventBus.cpp
        src/EventDispatcher.cpp
)

target_include_directories(domain
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(domain PUBLIC cxx_std_20)
