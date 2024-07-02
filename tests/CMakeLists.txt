add_executable(
        test_cell
        test_cell.cpp
)
target_link_libraries(
        test_cell
        PRIVATE CA
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_cell)