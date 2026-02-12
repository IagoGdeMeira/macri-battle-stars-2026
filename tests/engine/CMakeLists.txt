add_executable(engine_tests
    EventBusTests.cpp
)

target_link_libraries(engine_tests
    PRIVATE
        engine
        Catch2::Catch2WithMain
)

add_test(NAME engine_tests COMMAND engine_tests)
