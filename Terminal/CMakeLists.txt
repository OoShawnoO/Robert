cmake_minimum_required(VERSION 3.20)

set(TARGET Terminal)

project(${TARGET})
# ThirdParty
find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        OpenGL
        REQUIRED
)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE UIS ui/*.ui)
file(GLOB_RECURSE RES resources/*.qrc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNODE_EDITOR_SHARED")

# Third Party
find_package(OpenCV REQUIRED)

# Include
include_directories(${OPENCV_INCLUDES})
include_directories(include)
include_directories(../Utils/include)
include_directories(include/editor)
include_directories(thirdparty/include/QtNodes/internal)
include_directories(thirdparty/include/QtNodes)
#include_directories(thirdparty/src/QtNodes)
# Library
link_directories(lib)
link_directories(../Utils/lib)
# Binary Output Path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
# Add Executable
add_executable(${TARGET}
        ${HEADERS} ${SRCS} ${UTILS_SRCS} ${UIS} ${RES}
)
# Link Libraries
target_link_libraries(
        ${TARGET}
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL
        QtNodes
        ${OpenCV_LIBS}
        Utils
)
