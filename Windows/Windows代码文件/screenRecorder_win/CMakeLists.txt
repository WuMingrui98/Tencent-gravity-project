cmake_minimum_required(VERSION 2.8.12.2)
set(PROJECT_NAME screenRecorder_win)
project(${PROJECT_NAME} C)

set(CMAKE_C_STANDARD 99)
set(INC_DIR
        C:/Users/61052/Desktop/ffmpeg-4.3.2-2021-02-27-full_build-shared/include
        C:/Users/61052/Desktop/SDL2-2.0.18/x86_64-w64-mingw32/include)
set(LINK_DIR
        C:/Users/61052/Desktop/ffmpeg-4.3.2-2021-02-27-full_build-shared/lib
        C:/Users/61052/Desktop/SDL2-2.0.18/x86_64-w64-mingw32/lib)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})

add_executable(${PROJECT_NAME} main.c captureScreen.c)
target_link_libraries(${PROJECT_NAME} mingw32 SDL2main SDL2 avdevice avcodec avformat swscale avutil)