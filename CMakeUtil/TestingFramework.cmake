FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(CTest)


function(add_test_executable)
    if(NOT ARGC)
        message(FATAL_ERROR "add_test_executable requires a target name.")
    endif()

    set(target_name ${ARGV0})

    cmake_parse_arguments(
            TEST_ARGS       # The variable to store the parsed arguments
            ""              # Options (boolean flags, not used here)
            ""              # One-value arguments (TARGET_NAME is handled above)
            "SOURCES"       # Multi-value arguments
            ${ARGN}         # The list of arguments passed to the function
    )

    if(NOT TEST_ARGS_SOURCES)
        message(FATAL_ERROR "add_test_executable requires SOURCES.")
    endif()

    add_executable(${target_name} ${TEST_ARGS_SOURCES})

    target_link_libraries(${target_name}
            PRIVATE
            ${component}
            gtest_main
    )

    add_test(
            NAME ${target_name}_Tests
            COMMAND $<TARGET_FILE:${target_name}>
    )
endfunction()