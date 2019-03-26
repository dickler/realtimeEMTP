cmake_minimum_required (VERSION 2.6)
project (Simulemt)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
include(CheckLinkerFlag)
include(LibraryUtils)
find_package(MKL)

FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Quick REQUIRED)
FIND_PACKAGE(Qt5Qml REQUIRED)

INCLUDE_DIRECTORIES(
    ${qtvulkan_SOURCE_DIR}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
    ${Qt5Qml_INCLUDE_DIRS}
)

SET(qml_LIBS
 ${Qt5Widgets_LIBRARIES}
 ${Qt5Quick_LIBRARIES}
 ${Qt5Core_LIBRARIES}
 ${Qt5Gui_LIBRARIES}
 ${QT5Qml_LIBRARIES}
)







message(STATUS "MKLFOUND: " ${MKL_FOUND})


file(GLOB SOURCES ${SOURCES} "src/*.cpp" "include/*.h" kernel.cpp "Build/netlist.txt")


include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${MKL_INCLUDE_DIRS})

add_executable(Simulemt ${SOURCES})
target_link_libraries(Simulemt ${MKL_LIBRARIES})



TARGET_LINK_LIBRARIES(Simulemt ${qml_LIBS})

