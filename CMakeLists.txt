cmake_minimum_required(VERSION 3.30)
project(SmartPhone-new)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE SOURCES Src/*.cpp Src/*.h)
add_executable(phone main.cpp ${SOURCES})
target_include_directories(phone PRIVATE Src)

find_package(Qt5 COMPONENTS
        Core
        Widgets
        SerialPort
        WebEngineWidgets
        REQUIRED)
target_link_libraries(phone
        Qt5::Core
        Qt5::Widgets
        Qt5::SerialPort
        Qt5::WebEngineWidgets
        )

find_package(spdlog REQUIRED)
target_link_libraries(phone spdlog::spdlog)

set(QTAWESOME_PATH ${CMAKE_SOURCE_DIR}/Src/libs/QtAwesome)
add_subdirectory(${QTAWESOME_PATH})
target_link_libraries(phone QtAwesome)
add_definitions(-DQT_AWESOME_PRO)

add_library(qtwaitingspinner Src/gui/utils/waitingspinnerwidget.h Src/gui/utils/waitingspinnerwidget.cpp)
target_link_libraries(qtwaitingspinner Qt5::Core Qt5::Widgets)
target_link_libraries(phone qtwaitingspinner)
