add_library(domain STATIC)

target_sources(domain
    PRIVATE
        src/World.cpp
)

target_include_directories(domain
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(domain PUBLIC cxx_std_20)
