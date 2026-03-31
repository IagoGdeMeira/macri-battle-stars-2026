add_executable(engine_tests
    CommandBufferTests.cpp
    EngineTests.cpp
    EventBusTests.cpp
    InputMapperTests.cpp
    InputSystemTests.cpp
    MovementSystemTests.cpp
    SceneManagerTests.cpp
    SceneTests.cpp
    SystemManagerTests.cpp
)

target_link_libraries(engine_tests
    PRIVATE
        engine
        Catch2::Catch2WithMain
)

add_test(NAME engine_tests COMMAND engine_tests)
