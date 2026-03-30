add_executable(platform_tests
    SDLInputAdapterTests.cpp
)

target_link_libraries(platform_tests
    PRIVATE
        platform
        Catch2::Catch2WithMain
)

add_test(NAME platform_tests COMMAND platform_tests)

if(WIN32)
    copy_sdl_dlls(platform_tests)

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(_sdl_arch x64)
    else()
        set(_sdl_arch x86)
    endif()

    set(_platform_test_path
        "${PROJECT_SOURCE_DIR}/external/SDL2/bin/${_sdl_arch}\\;${PROJECT_SOURCE_DIR}/external/SDL2_image/bin/${_sdl_arch}\\;${PROJECT_SOURCE_DIR}/external/SDL2_ttf/bin/${_sdl_arch}\\;${PROJECT_SOURCE_DIR}/external/SDL2_mixer/bin/${_sdl_arch}\\;$ENV{PATH}"
    )

    set_tests_properties(platform_tests PROPERTIES
        ENVIRONMENT "PATH=${_platform_test_path}"
    )
endif()
