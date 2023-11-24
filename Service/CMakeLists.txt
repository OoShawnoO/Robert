cmake_minimum_required(VERSION 3.20)

set(TARGET Service)

project(${TARGET})

# Third Party
find_package(OpenCV REQUIRED)

# Include
include_directories(include)
include_directories(../Utils/include)
include_directories(${OPENCV_INCLUDES})
# Library
link_directories(lib)
link_directories(../Utils/lib)
# Source
aux_source_directory(src SRC)
aux_source_directory(../Utils/src SRC)
# Binary Output Path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
# Add Executable
add_executable(${TARGET} ${SRC})
# Link Libraries
target_link_libraries(${TARGET} ${OpenCV_LIBS})
