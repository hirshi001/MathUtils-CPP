# CMakeLists.txt for the test executable

# Define the test executable
add_executable(test_time
        main.cpp
)

# Link the test executable to your Vector library and the Google Test libraries.
# gtest_main is required to get a main function for your tests.
target_link_libraries(test_time
        PRIVATE
        ${component}
        gtest_main
)

# This command ensures that the test executable is run as part of the CTest suite.
# This is a common practice for running unit tests in CMake projects.
include(CTest)
add_test(
        NAME TimeTests
        COMMAND $<TARGET_FILE:test_time>
)
