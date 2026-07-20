add_library(domain STATIC)

target_sources(domain
    PRIVATE
        src/ComponentManager.cpp
        src/EntityManager.cpp
        src/Logger.cpp
        src/StateId.cpp
)

target_include_directories(domain
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/components
        ${CMAKE_CURRENT_SOURCE_DIR}/events
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/value_objects
)

target_compile_features(domain PUBLIC cxx_std_20)
