cmake_minimum_required(VERSION 3.20)

set(TARGET Service)

project(${TARGET})

# Third Party
find_package(OpenCV REQUIRED)
set(ONNX_PATH ~/env/onnxruntime-linux-x64-gpu-1.15.1)
# Include
include_directories(include)
include_directories(../Utils/include)
include_directories(${OPENCV_INCLUDES})
include_directories(${ONNX_PATH}/include)
# Library
link_directories(lib)
link_directories(../Utils/lib)
link_directories(${ONNX_PATH}/lib)
# Source
aux_source_directory(src SRC)
# Binary Output Path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
# Add Executable
add_executable(${TARGET} ${SRC})
# Link Libraries
target_link_libraries(${TARGET} ${OpenCV_LIBS} onnxruntime Utils)
