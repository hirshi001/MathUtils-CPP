set(component Test)

add_library(${component} INTERFACE)

# Define include directories
target_include_directories(${component}
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Install the target so it can be exported later
install(TARGETS ${component}
        EXPORT ${export_set}
        COMPONENT ${component}
)

# Install the public headers for the component.
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${project}"
        COMPONENT ${component}
)


add_subdirectory(test)