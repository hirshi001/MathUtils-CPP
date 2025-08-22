# CMakeLists.txt for the test executable

add_test_executable(test_vector
        SOURCES
        matrix_tests.cpp
        vector_tests.cpp
)

target_link_libraries(test_vector PRIVATE Vector)
