add_library(nodesoup algebra.cpp fruchterman_reingold.cpp kamada_kawai.cpp
                     layout.cpp nodesoup.cpp)
add_library(nodesoup::nodesoup ALIAS nodesoup)

target_include_directories(
  nodesoup
  PUBLIC $<BUILD_INTERFACE:${nodesoup_SOURCE_DIR}/include>
  PRIVATE $<BUILD_INTERFACE:${nodesoup_SOURCE_DIR}/src>)

target_compile_features(nodesoup PUBLIC cxx_std_14)

set_target_properties(
  nodesoup PROPERTIES POSITION_INDEPENDENT_CODE ON
                      PUBLIC_HEADER ${nodesoup_SOURCE_DIR}/include/nodesoup.hpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(nodesoup PRIVATE -Wall -Wextra)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(nodesoup PRIVATE -Wall -Wextra)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(nodesoup PRIVATE /W4)
  target_compile_definitions(nodesoup PUBLIC _USE_MATH_DEFINES)
endif()
