add_executable(engine_tests
    Camera2DTests.cpp
    CommandBufferTests.cpp
    DataUtilsTests.cpp
    EngineTests.cpp
    EventBusTests.cpp
    InputBindingLoaderTests.cpp
    InputBufferSystemTests.cpp
    InputManagerTests.cpp
    InputMapperTests.cpp
    InputSystemTests.cpp
    ResourceLoaderTests.cpp
    ResourceManagerTests.cpp
    SceneFactoryTests.cpp
    SceneManagerTests.cpp
    SceneTests.cpp
    SystemManagerTests.cpp
    TextureLoaderTests.cpp
)

target_link_libraries(engine_tests
    PRIVATE
        Catch2::Catch2WithMain
        engine
        tests_common
)

add_test(NAME engine_tests COMMAND engine_tests)
