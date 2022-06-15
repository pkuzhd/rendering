cmake_minimum_required(VERSION 3.20)
project(rendering)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 14)
find_package(Boost REQUIRED COMPONENTS chrono filesystem system program_options regex timer)
find_package(OpenCV 3 REQUIRED core imgproc imgcodecs highgui objdetect video)
set(OPENCV_COMPONENTS
        opencv_core
        opencv_imgproc
        opencv_imgcodecs
        opencv_highgui
        opencv_objdetect
        opencv_video)

find_package(glfw3)
find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)


add_executable(rendering
        main.cpp
        glad.c
        )

add_executable(point
        point.cpp
        glad.c
        )

target_link_libraries(
        rendering
        Boost::filesystem
        Boost::system
        Boost::program_options
        Boost::timer
        Boost::chrono
        glfw
        OpenGL::OpenGL
        OpenGL::EGL
        ${OPENCV_COMPONENTS}
)
target_link_libraries(
        point
        Boost::filesystem
        Boost::system
        Boost::program_options
        Boost::timer
        Boost::chrono
        glfw
        OpenGL::OpenGL
        OpenGL::EGL
        ${OPENCV_COMPONENTS}
)
