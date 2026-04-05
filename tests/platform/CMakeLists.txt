add_executable(platform_tests
    JsonNodeTests.cpp
    JsonParserTests.cpp
    SDLInputAdapterTests.cpp
    SDLKeyTranslatorTests.cpp
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
