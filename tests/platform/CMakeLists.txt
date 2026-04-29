add_executable(platform_tests
    JsonNodeTests.cpp
    JsonParserTests.cpp
    SDLKeyboardAdapterTests.cpp
    SDLMouseAdapterTests.cpp
    SDLGamepadAdapterTests.cpp
    SDLKeyTranslatorTests.cpp
    SDLRendererTests.cpp
    SDLSystemInitializerTests.cpp
    SDLWindowTests.cpp
)

target_link_libraries(platform_tests
    PRIVATE
        platform
        Catch2::Catch2WithMain
)

add_test(NAME platform_tests COMMAND platform_tests)

if(WIN32)
    copy_sdl_dlls(platform_tests)
endif()
