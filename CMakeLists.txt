cmake_minimum_required(VERSION 3.0)
project(ClothSimulator)

set(CMAKE_AUTOMOC ON)

include_directories(include)
set(HEADER
        include/Vertex.h
        include/Spring.h
        include/Mesh.h
        include/Cloth.h
        include/RenderWidget.h
        include/RenderWindow.h
)
aux_source_directory(src SOURCE)
add_executable(FastSpringMass
        ${HEADER}
        ${SOURCE}
)

# Eigen
find_package(Eigen3)
include_directories(${EIGEN3_INCLUDE_DIR})

# Qt
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
target_link_libraries(
        FastSpringMass
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)