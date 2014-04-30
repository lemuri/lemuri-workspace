project(lemuri-workspace)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/)

find_package(Qt5 COMPONENTS
    Core
    DBus
    Quick
    Widgets
    Compositor
)

# Find pkg-config
find_package(PkgConfig "0.22" REQUIRED)

# Find EGL libraries
pkg_check_modules(GL gl REQUIRED)
pkg_check_modules(EGL egl REQUIRED)

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Core_DEFINITIONS} -DQT_COMPOSITOR_QUICK)
add_definitions(-std=gnu++11)

## Enable Qt's automoc
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(APP_VERSION 0.0.1)

# Generate config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include_directories(${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_subdirectory(src)
