cmake_minimum_required(VERSION 3.14...3.18)

project(
  nodesoup
  VERSION 0.1.0
  DESCRIPTION "Force-directed graph layout"
  HOMEPAGE_URL "https://github.com/olvb/nodesoup"
  LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  option(BUILD_DEMO "Build the demo" ON)
  option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
endif()

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(
    STATUS
      "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE
      "${default_build_type}"
      CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

add_subdirectory(src)

include(GNUInstallDirs)
install(
  TARGETS nodesoup
  EXPORT nodesoup-targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES
  DESTINATION include
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nodesoup)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  nodesoup-config-version.cmake
  VERSION ${PACKAGE_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(
  EXPORT nodesoup-targets
  FILE nodesoup-config.cmake
  NAMESPACE nodesoup::
  DESTINATION lib/cmake/nodesoup)

export(
  TARGETS nodesoup
  NAMESPACE nodesoup::
  FILE nodesoup-targets.cmake)

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR nodesoup_BUILD_DEMO)
   AND BUILD_DEMO)
  add_subdirectory(demo)
endif()
