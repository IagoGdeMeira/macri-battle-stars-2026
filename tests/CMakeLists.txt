enable_testing()

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(Catch2)

add_library(tests_common INTERFACE)

target_include_directories(tests_common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/stubs
    ${CMAKE_SOURCE_DIR}/src
)

add_subdirectory(app)
add_subdirectory(domain)
add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(platform)
