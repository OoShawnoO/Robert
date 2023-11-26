cmake_minimum_required(VERSION 3.20)

set(TARGET Terminal)

project(${TARGET})
# ThirdParty
find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE UIS ui/*.ui)
file(GLOB_RECURSE RES resources/*.qrc)


# Include
include_directories(include)
include_directories(../Utils/include)
# Library
link_directories(lib)
link_directories(../Utils/lib)
# Binary Output Path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
# Add Executable
add_executable(${TARGET} ${HEADERS} ${SRCS} ${UIS} ${RES})
# Link Libraries
target_link_libraries(
        ${TARGET}
        Qt5::Core Qt5::Gui Qt5::Widgets
)
