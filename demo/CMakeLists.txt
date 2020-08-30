find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIRO REQUIRED IMPORTED_TARGET cairo)

add_executable(dot2png dot2png.cpp)

target_link_libraries(dot2png PRIVATE nodesoup PkgConfig::CAIRO)
