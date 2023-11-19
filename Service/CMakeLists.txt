cmake_minimum_required(VERSION 3.20)

set(TARGET Service)

project(${TARGET})
# Include
include_directories(include)
include_directories(../Utils/include)
# Library
link_directories(lib)
link_directories(../Utils/lib)
# Source
aux_source_directory(src SRC)
# Binary Output Path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
# Add Executable
# add_executable(${TARGET} ${SRC})
# Link Libraries
# target_link_libraries(${TARGET})
