add_executable(platform_tests
    JsonNodeTests.cpp
    JsonParserTests.cpp
    SDLFontTests.cpp
    SDLKeyboardAdapterTests.cpp
    SDLMouseAdapterTests.cpp
    SDLGamepadAdapterTests.cpp
    SDLPlatformEventProviderTests.cpp
    SDLKeyTranslatorTests.cpp
    SDLMusicTests.cpp
    SDLMusicFactoryTests.cpp
    SDLRendererTests.cpp
    SDLSoundTests.cpp
    SDLSoundFactoryTests.cpp
    SDLSystemAdapterTests.cpp
    SDLSystemInitializerTests.cpp
    SDLTextureTests.cpp
    SDLTextureFactoryTests.cpp
    SDLPlatformFactoryTests.cpp
    SDLWindowTests.cpp
)

target_link_libraries(platform_tests
    PRIVATE
        Catch2::Catch2WithMain
        platform
        tests_common
)

if(WIN32)
    target_link_libraries(platform_tests PRIVATE SDL2_mixer)
else()
    target_link_libraries(platform_tests PRIVATE SDL2_mixer::SDL2_mixer)
endif()

add_test(NAME platform_tests COMMAND platform_tests)

if(WIN32)
    copy_sdl_dlls(platform_tests)
endif()
