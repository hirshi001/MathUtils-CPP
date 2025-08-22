cmake_minimum_required(VERSION 3.5)
set(project MathUtils)
set(version 1.0.0)
project(${project} VERSION ${version} LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 23)

set(export_set "${project}-targets")

install(EXPORT "${export_set}"
        FILE "${project}.cmake"
        NAMESPACE ${project}::
        DESTINATION lib/cmake/${project}
)

include(FetchContent)
include(CMakeUtil/TestingFramework.cmake)
include(CmakeUtil/Component.cmake)

# Add Subdirectories here
add_subdirectory(Vector)
add_subdirectory(Time)

include(CMakePackageConfigHelpers)

set(project_config_name "${project}-config.cmake")
configure_package_config_file(
        "${project_config_name}.in"
        "${CMAKE_BINARY_DIR}/${project_config_name}"
        INSTALL_DESTINATION lib/cmake/${project}
)

write_basic_package_version_file(
        "${CMAKE_BINARY_DIR}/${project}-config-version.cmake"
        VERSION ${version}
        COMPATIBILITY AnyNewerVersion
)
