cmake_minimum_required(VERSION 3.30)
project(SmartPhone-new)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE SOURCES Src/*.cpp Src/*.h)

find_package(Qt6 COMPONENTS
        Core
        Widgets
        SerialPort
        WebEngineWidgets
        StateMachine
        REQUIRED)

QT_ADD_RESOURCES(RC_SRC "resources.qrc")

add_executable(phone main.cpp ${SOURCES} ${RC_SRC})
target_include_directories(phone PRIVATE Src)

target_link_libraries(phone
        Qt6::Core
        Qt6::Widgets
        Qt6::SerialPort
        Qt6::WebEngineWidgets
        Qt6::StateMachine
        )

find_package(spdlog REQUIRED)
target_link_libraries(phone spdlog::spdlog)

set(QTAWESOME_PATH ${CMAKE_SOURCE_DIR}/Src/libs/QtAwesome)
add_subdirectory(${QTAWESOME_PATH})
target_link_libraries(phone QtAwesome)
add_definitions(-DQT_AWESOME_PRO)

add_library(qtwaitingspinner Src/gui/utils/waitingspinnerwidget.h Src/gui/utils/waitingspinnerwidget.cpp)
target_link_libraries(qtwaitingspinner Qt6::Core Qt6::Widgets)
target_link_libraries(phone qtwaitingspinner)

add_library(pdu Src/libs/pdulib/pdulib.h Src/libs/pdulib/pdulib.cpp)
target_link_libraries(phone pdu)
