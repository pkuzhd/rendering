cmake_minimum_required(VERSION 3.10)
project(rendering)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(CMAKE_CXX_STANDARD 14)
find_package(glfw3)
find_package(jsoncpp)
find_package(OpenCV)
set(OPENCV_COMPONENTS
        opencv_core
        opencv_imgproc
        opencv_imgcodecs
        opencv_highgui
        opencv_objdetect
        opencv_video)

include_directories(../pipe_transmission)
add_subdirectory(../pipe_transmission pipe_transmission)
#add_library(TransLib ../pipe_transmission/utils/ImageSender.cpp ../pipe_transmission/utils/RGBDReceiver.cpp)

add_executable(rendering
        main.cpp
        glad.c
        ply/PlyReader.cpp ply/system.cpp ply/plyUtils.cpp
        utils/Renderer.cpp
        utils/ThreadPool.cpp utils/ThreadPool.h)

add_executable(videoRendering
        videoRendering.cpp
        glad.c
        ply/PlyReader.cpp ply/system.cpp ply/plyUtils.cpp
        utils/Renderer.cpp
        utils/ThreadPool.cpp utils/ThreadPool.h)

add_executable(TestImage
        TestImage.cpp
        utils/ThreadPool.cpp utils/ThreadPool.h)
add_executable(test
        test.cpp
        glad.c
        ply/PlyReader.cpp ply/system.cpp ply/plyUtils.cpp
        utils/Renderer.cpp
        utils/ThreadPool.cpp utils/ThreadPool.h)

add_executable(point
        point.cpp
        glad.c
        utils/ThreadPool.cpp utils/ThreadPool.h)
add_executable(blend
        blend.cpp
        glad.c
        utils/ThreadPool.cpp utils/ThreadPool.h)
add_executable(background
        background.cpp
        glad.c
        ply/PlyReader.cpp ply/system.cpp ply/plyUtils.cpp
        utils/loadCamPara.cpp utils/Renderer.cpp
        utils/ThreadPool.cpp utils/ThreadPool.h)

add_executable(plyreader plyreader.cpp ply/PlyReader.cpp ply/system.cpp ply/plyUtils.cpp utils/ThreadPool.cpp utils/ThreadPool.h)


target_link_libraries(
        rendering
        glfw
        jsoncpp
        #        ${OPENCV_COMPONENTS}
        TransLib
        pthread
)
target_link_libraries(
        videoRendering
        glfw
        jsoncpp
        #        ${OPENCV_COMPONENTS}
        TransLib
        pthread
)
target_link_libraries(
        TestImage
        jsoncpp
        ${OPENCV_COMPONENTS}
        TransLib
        pthread
)
target_link_libraries(
        point
        glfw
        jsoncpp
        ${OPENCV_COMPONENTS}
)

target_link_libraries(
        blend
        glfw
        jsoncpp
        ${OPENCV_COMPONENTS}
)
target_link_libraries(
        background
        glfw
        jsoncpp
        ${OPENCV_COMPONENTS}
)

target_link_libraries(
        test
        glfw
        jsoncpp
        ${OPENCV_COMPONENTS}
        #        ${OpenCV_LIBS}
)
