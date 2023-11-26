cmake_minimum_required(VERSION 3.20)

set(TARGET Terminal)

project(${TARGET})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# ThirdParty
find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED
)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)
set(CMAKE_AUTOUIC_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE FORMS ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)
file(GLOB_RECURSE RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)

# Include
include_directories(include)
include_directories(../Utils/include)
# Library
link_directories(lib)
link_directories(../Utils/lib)
# Source
aux_source_directory(src SRC)
aux_source_directory(ui SRC)
aux_source_directory(resources SRC)
# Binary Output Path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
# Add Executable
add_executable(${TARGET} ${SRC} ${RESOURCES} ${FORMS})
# Link Libraries
target_link_libraries(
        ${TARGET}
        Qt5::Core Qt5::Gui Qt5::Widgets
)
