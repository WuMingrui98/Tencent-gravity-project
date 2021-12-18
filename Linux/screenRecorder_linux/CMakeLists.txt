cmake_minimum_required(VERSION 2.8.12.2)
set(PROJECT_NAME screenRecorder)
project(${PROJECT_NAME} C)

set(CMAKE_C_STANDARD 99)
set(INC_DIR
        /usr/include)
set(LINK_DIR
        )

include_directories(${INC_DIR})
link_directories(${LINK_DIR})

add_executable(${PROJECT_NAME} main.c captureScreen.c)
target_link_libraries(${PROJECT_NAME}  SDL avdevice avcodec avformat swscale avutil)