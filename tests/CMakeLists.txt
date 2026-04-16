enable_testing()

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(Catch2)

add_subdirectory(app)
add_subdirectory(domain)
add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(platform)
