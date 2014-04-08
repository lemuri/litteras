project(Litteras)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/)

find_package(Qt5 COMPONENTS
    Core
    DBus
    Quick
    Widgets
    Xml
    Network
    WebKit
    WebKitWidgets
)

find_package(EwsQt5 REQUIRED)
find_package(Xapian 1.2 REQUIRED)
add_definitions("-fexceptions")

set(APP_VERSION 0.1.0)

## Enable Qt's automoc generator
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Generate config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include(FindPkgConfig)

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(-fPIC -std=gnu++11)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libews-qt
    ${CMAKE_CURRENT_BINARY_DIR}
    ${EwsQt5_INCLUDE_DIR}
    ${XAPIAN_INCLUDE_DIR}
)

add_subdirectory(src)
