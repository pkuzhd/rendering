cmake_minimum_required(VERSION 3.10)
project(rendering)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 14)
find_package(glfw3)

add_executable(rendering
        main.cpp
        glad.c
        ply/PlyReader.cpp ply/system.cpp ply/plyUtils.cpp
#        utils/Renderer.cpp utils/Renderer.h
        )

add_executable(point
        point.cpp
        glad.c
        )
add_executable(blend
        blend.cpp
        glad.c
        )
add_executable(background
        background.cpp
        glad.c
        ply/PlyReader.cpp ply/system.cpp ply/plyUtils.cpp
        utils/loadCamPara.cpp utils/Renderer.cpp
        )

add_executable(plyreader plyreader.cpp ply/PlyReader.cpp ply/system.cpp ply/plyUtils.cpp)


target_link_libraries(
        rendering
        glfw
)
target_link_libraries(
        point
        glfw
)

target_link_libraries(
        blend
        glfw
)
target_link_libraries(
        background
        glfw
)
