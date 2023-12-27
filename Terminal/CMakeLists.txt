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
#file(GLOB_RECURSE UTILS_SRCS ../Utils/src/*.cpp)
file(GLOB_RECURSE UIS ui/*.ui)
file(GLOB_RECURSE RES resources/*.qrc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNODE_EDITOR_SHARED")

set(QTNODES_CPP_SOURCE_FILES
        thirdparty/src/QtNodes/AbstractGraphModel.cpp
        thirdparty/src/QtNodes/AbstractNodeGeometry.cpp
        thirdparty/src/QtNodes/BasicGraphicsScene.cpp
        thirdparty/src/QtNodes/ConnectionGraphicsObject.cpp
        thirdparty/src/QtNodes/ConnectionPainter.cpp
        thirdparty/src/QtNodes/ConnectionState.cpp
        thirdparty/src/QtNodes/ConnectionStyle.cpp
        thirdparty/src/QtNodes/DataFlowGraphModel.cpp
        thirdparty/src/QtNodes/DataFlowGraphicsScene.cpp
        thirdparty/src/QtNodes/DefaultHorizontalNodeGeometry.cpp
        thirdparty/src/QtNodes/DefaultVerticalNodeGeometry.cpp
        thirdparty/src/QtNodes/Definitions.cpp
        thirdparty/src/QtNodes/GraphicsView.cpp
        thirdparty/src/QtNodes/GraphicsViewStyle.cpp
        thirdparty/src/QtNodes/NodeDelegateModelRegistry.cpp
        thirdparty/src/QtNodes/NodeConnectionInteraction.cpp
        thirdparty/src/QtNodes/NodeDelegateModel.cpp
        thirdparty/src/QtNodes/NodeGraphicsObject.cpp
        thirdparty/src/QtNodes/DefaultNodePainter.cpp
        thirdparty/src/QtNodes/NodeState.cpp
        thirdparty/src/QtNodes/NodeStyle.cpp
        thirdparty/src/QtNodes/StyleCollection.cpp
        thirdparty/src/QtNodes/UndoCommands.cpp
        thirdparty/src/QtNodes/locateNode.cpp
)

set(QTNODES_HPP_HEADER_FILES
        thirdparty/include/QtNodes/internal/AbstractGraphModel.hpp
        thirdparty/include/QtNodes/internal/AbstractNodeGeometry.hpp
        thirdparty/include/QtNodes/internal/AbstractNodePainter.hpp
        thirdparty/include/QtNodes/internal/BasicGraphicsScene.hpp
        thirdparty/include/QtNodes/internal/Compiler.hpp
        thirdparty/include/QtNodes/internal/ConnectionGraphicsObject.hpp
        thirdparty/include/QtNodes/internal/ConnectionIdHash.hpp
        thirdparty/include/QtNodes/internal/ConnectionIdUtils.hpp
        thirdparty/include/QtNodes/internal/ConnectionState.hpp
        thirdparty/include/QtNodes/internal/ConnectionStyle.hpp
        thirdparty/include/QtNodes/internal/DataFlowGraphicsScene.hpp
        thirdparty/include/QtNodes/internal/DataFlowGraphModel.hpp
        thirdparty/include/QtNodes/internal/DefaultNodePainter.hpp
        thirdparty/include/QtNodes/internal/Definitions.hpp
        thirdparty/include/QtNodes/internal/Export.hpp
        thirdparty/include/QtNodes/internal/GraphicsView.hpp
        thirdparty/include/QtNodes/internal/GraphicsViewStyle.hpp
        thirdparty/include/QtNodes/internal/locateNode.hpp
        thirdparty/include/QtNodes/internal/NodeData.hpp
        thirdparty/include/QtNodes/internal/NodeDelegateModel.hpp
        thirdparty/include/QtNodes/internal/NodeDelegateModelRegistry.hpp
        thirdparty/include/QtNodes/internal/NodeGraphicsObject.hpp
        thirdparty/include/QtNodes/internal/NodeState.hpp
        thirdparty/include/QtNodes/internal/NodeStyle.hpp
        thirdparty/include/QtNodes/internal/OperatingSystem.hpp
        thirdparty/include/QtNodes/internal/QStringStdHash.hpp
        thirdparty/include/QtNodes/internal/QUuidStdHash.hpp
        thirdparty/include/QtNodes/internal/Serializable.hpp
        thirdparty/include/QtNodes/internal/Style.hpp
        thirdparty/include/QtNodes/internal/StyleCollection.hpp
        thirdparty/src/QtNodes/ConnectionPainter.hpp
        thirdparty/src/QtNodes/DefaultHorizontalNodeGeometry.hpp
        thirdparty/src/QtNodes/DefaultVerticalNodeGeometry.hpp
        thirdparty/src/QtNodes/NodeConnectionInteraction.hpp
        thirdparty/src/QtNodes/UndoCommands.hpp
)

# Third Party
find_package(OpenCV REQUIRED)

# Include
include_directories(${OPENCV_INCLUDES})
include_directories(include)
include_directories(../Utils/include)
include_directories(include/editor)
include_directories(thirdparty/include/QtNodes/internal)
include_directories(thirdparty/include/QtNodes)
include_directories(thirdparty/src/QtNodes)
# Library
link_directories(lib)
link_directories(../Utils/lib)
# Binary Output Path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
# Add Executable
add_executable(${TARGET}
        ${QTNODES_CPP_SOURCE_FILES} ${QTNODES_HPP_HEADER_FILES}
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
