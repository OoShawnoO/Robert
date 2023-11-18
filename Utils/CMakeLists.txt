cmake_minimum_required(VERSION 3.20)

set(TARGET Utils)

project(${TARGET})
# Library Output Path
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

# Third Party
find_package(OpenCV REQUIRED)
# Include
include_directories(include)
include_directories(${OPENCV_INCLUDES})
# Library
link_directories(lib)
# Source
aux_source_directory(src SRC)
# Add Executable
add_library(${TARGET} ${SRC})
# Link Libraries
target_link_libraries(${TARGET} ${OpenCV_LIBS})
