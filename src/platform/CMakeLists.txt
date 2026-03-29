add_library(platform STATIC)

target_sources(platform
    PRIVATE
        src/SDLWindow.cpp
        src/SDLRenderer.cpp
        src/SDLInputAdapter.cpp
)

target_include_directories(platform
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(WIN32)

    target_link_libraries(platform
        PUBLIC
            SDL2
        PRIVATE
            SDL2_image
            SDL2_ttf
            SDL2_mixer
    )

elseif(UNIX)

    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_mixer REQUIRED)

    target_link_libraries(platform
        PUBLIC
            SDL2::SDL2
        PRIVATE
            SDL2_image::SDL2_image
            SDL2_ttf::SDL2_ttf
            SDL2_mixer::SDL2_mixer
    )

endif()

target_link_libraries(platform
    PUBLIC
        engine
        domain
)

target_compile_features(platform PUBLIC cxx_std_20)
