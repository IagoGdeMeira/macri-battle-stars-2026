add_executable(app_tests
    ApplicationTests.cpp
)

target_link_libraries(app_tests
    PRIVATE
        Catch2::Catch2WithMain
        app
        tests_common
)

add_test(NAME app_tests COMMAND app_tests)

if(WIN32)
    copy_sdl_dlls(app_tests)
endif()
