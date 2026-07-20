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
        Catch2::Catch2WithMain
        domain
        tests_common
)

add_test(NAME domain_tests COMMAND domain_tests)
