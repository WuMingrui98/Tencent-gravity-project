# Linux(Ubuntu 20.04.1 LTS)平台实现录屏并推流

该程序使用FFmpeg库和SDL1.2库，通过x11grab抓取Linux屏幕，该程序能够实现Linux屏幕的录制，并通过rtmp协议推流到流媒体服务器。

>  通过Nginx搭建流媒体服务器请参考：[在Ubuntu 14 上安装 Nginx-RTMP 流媒体服务器 ](https://www.cnblogs.com/cocoajin/p/4353767.html)

## 1. 引入依赖库

程序中使用了FFmpeg库与SDL1.2库，因此需要在系统中安装FFmpeg与SDL1.2库。

安装方式有两种：

1. 下载源码并在本地编译。

   * [下载FFmpeg](http://ffmpeg.org/download.html)

     >  编译时需要配置enable-libx264，因此在安装FFmpeg之前先安装libx264库，不然会报错
     >
     > 官网下载的FFmpeg 4.4.1源码，我在使用过程中也出现了问题，Ubuntu是通过x11grab抓取屏幕的，抓取的像素格式是BGR0，因为需要用到h264对抓取的原图像进行编码，h264不支持BGR0这种像素格式，因此我需要利用FFmpeg提供的sws_scale()函数将BGR0格式转换成YUV420P，但是转换后会出现图像花屏的情况，我自己也尝试按照BGR0和YUV420P在FFmpeg中的数据存储格式写了一个转换函数，一样出现了花屏的情况，目前没有找到花屏的原因，因此替换了更老的FFmpeg版本4.2.4，然后功能就正常了。如果有高手能解决这个问题，希望不吝赐教。

   * [下载SDL](http://www.libsdl.org/download-2.0.php)

     > 目前官网下载提供的是SDL2源码，因为我在编程过程中发现SDL2与FFmpeg在Ubuntu 20.04.1 LTS 上兼容的不是很好，SDL2渲染出的显示窗口不能正常关闭，会出现死机的情况，这个情况在Windows10上没有出现。因为没有找到解决方法，因此替换了老版本的SDL1.2开发，窗口就没有出现不能正常关闭的情况，如果有高手能够解决这个问题，希望不吝赐教。

2. 通过apt包安装管理工具安装

   需要用到的包有：libsdl1.2-dev、libavutil-dev、libavformat-dev、libswscale-dev、libavdevice-dev、libavcodec-dev

   通过命令`sudo apt install libsdl1.2-dev libavutil-dev libavformat-dev libswscale-dev libavdevice-dev libavcodec-dev`安装这些包

## 2. 编译程序

如果采用apt安装依赖库的话，可以在终端中输入命令`gcc -o srceenRecorder captureScreen.c main.c -I /usr/include/  -lpthread -lSDL -lavcodec -lavformat -lswscale -lavdevice -lavutil`编译出可执行文件

由于本程序是通过Clion构建的，它支持cmake编译程序，因此也可以修改文件夹中的`CMakeLists.txt`文件，通过cmake编译程序。

```cmake
cmake_minimum_required(VERSION 2.8.12.2)
set(PROJECT_NAME screenRecorder_linux)
project(${PROJECT_NAME} C)

# 这里目录要替换成你们的电脑中安装资源的目录
# 设置引入头文件所在目录的别名
set(CMAKE_C_STANDARD 99)
set(INC_DIR /usr/include)
# 设置要引入的库所在目录的别名
set(LINK_DIR)
# 指定引入的头文件所在路径
include_directories(${INC_DIR})
# 指定引入的库文件所在路径
link_directories(${LINK_DIR})
# 构建可执行目标文件所需要的源代码文件
add_executable(${PROJECT_NAME} main.c captureScreen.c)
# 指定要链接的库(-l命令)
target_link_libraries(${PROJECT_NAME}  SDL avdevice avcodec avformat swscale avutil)
```

编译后的程序为：screenRecorder_linux

如果通过cmake编译，可以在`cmake-build-debug`目录下查看到编译出的程序

如果通过终端输入命令，则在当前目录下

该程序链接的是动态库(.so)文件，如果需要在其他环境运行本程序，需要将链接的动态库拷贝到程序所在文件夹，或者在要运行的环境中安装相应的FFmpeg和SDL库。如果程序链接的是静态库(.a)，则可以直接拷贝到其他Linux环境。但是有一点需要注意，**其他Linux系统使用的指令集需要编译环境相同**，比如ARM指令集编译的程序就不能在使用X86指令集的Linux上运行，需要在对应平台上重新编译。

## 3. 使用帮助

通过终端cd进入相应的文件夹，输入命令

`./screenRecorder_linux -? ` 或 `./screenRecorder_linux --help ` 获取帮助

> screenRecorder [option]...
>
> -h|--height        Specify the height of the output image of the recording screen. The default is 480.
>
> -w|--width         Specify the width of the output image of the recording screen. The default is 640.
>
> -a|--address       Specify the rtmp server address. The default is rtmp://110.40.193.165:1935/live/tencent
>
> -v|--video_size    Specify the resolution of the recorded video. It is automatically set according to the system. For example, the input format is 1920×1080.
>
> -i|--input         Specifies the input source for the video. The default is ':0'.
>
> -?|--help          Show the information

## 4. 程序思路

程序内有详细的注释，请参考注释。程序可以分为以下几个步骤，每个步骤有相应的函数。

> 1. 打开设备(x11grab)
> 2. 找到视频流的索引
> 3. 打开输入视频的解码器
> 4. 打开输出视频的编码器
> 5. 打开推流的输出上下文
> 6. 开启子线程，刷新SDL播放器
> 7.  录制屏幕，在SDL播放器中实时播放，推流到指定的rtmp服务器

## 5. 程序流程

![linux端流程图 (1)](https://tva1.sinaimg.cn/large/008i3skNly1gxh1r14ioej31ck0u0ae7.jpg)

## 6. 程序演示

![image-20211217111358164](https://tva1.sinaimg.cn/large/008i3skNly1gxgnqlq5v7j314c0u0th7.jpg)

## 7. 程序参考内容

1. [最简单的基于FFmpeg的AVDevice例子（屏幕录制）](https://blog.csdn.net/leixiaohua1020/article/details/39706721)
2. [最简单的基于FFmpeg的推流器（以推送RTMP为例）](https://blog.csdn.net/leixiaohua1020/article/details/39803457)

