cmake_minimum_required(VERSION 3.20)

project(neodencam VERSION 1.0.0 LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Locate libusb
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED IMPORTED_TARGET libusb-1.0)

add_library(neodencam SHARED ${CMAKE_CURRENT_SOURCE_DIR}/neodencam.c)
target_include_directories(neodencam PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(neodencam PkgConfig::LIBUSB)
set_target_properties(neodencam PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(neodencam PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(neodencam PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/neodencam.h)

include(GNUInstallDirs)
install(TARGETS neodencam
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
