add_executable(domain_tests
    ComponentManagerTests.cpp
    ComponentStorageTests.cpp
    EntityManagerTests.cpp
    EntityTests.cpp
    WorldTests.cpp
    ViewTests.cpp
)

target_link_libraries(domain_tests
    PRIVATE
        domain
        Catch2::Catch2WithMain
)

add_test(NAME domain_tests COMMAND domain_tests)
