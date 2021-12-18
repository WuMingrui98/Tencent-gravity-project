# Windows平台实现录屏并推流

## 一、Windows上使用FFmpeg和SDL2(使用 Cmake编译)

### 1. 资源下载

#### FFmpeg

[Download FFmpeg](http://ffmpeg.org/download.html)

![image-20211216233429955](https://tva1.sinaimg.cn/large/008i3skNgy1gxg3r20xfgj31qe0u0gru.jpg)

[Builds - CODEX FFMPEG @ gyan.dev](https://www.gyan.dev/ffmpeg/builds/)

![image-20211216234338970](https://tva1.sinaimg.cn/large/008i3skNgy1gxg3sfiybkj31kh0u0afe.jpg)



#### SDL2

[Simple DirectMedia Layer - SDL version 2.0.18 (stable) (libsdl.org)](http://www.libsdl.org/download-2.0.php)

![image-20211216234444047](https://tva1.sinaimg.cn/large/008i3skNgy1gxg3th4iedj31ka0u0n25.jpg)

#### MinGW

> MinGW，是Minimalist GNU for Windows的缩写。它是一个可自由使用和自由发布的Windows 特定头文件和使用GNU工具集导入库的集合，允许你在GNU/Linux和Windows平台生成本地的 Windows程序而不需要第三方C运行时（C Runtime）库。

https://sourceforge.net/projects/mingw-w64/

![image-20211216234543509](https://tva1.sinaimg.cn/large/008i3skNgy1gxg3uhv2jyj31e20u0af1.jpg)

### 2. Clion上配置cmake

> CMake是一个跨平台的安装（编译）工具，可以用简单的语句来描述所有平台的安装(编译过程)

Clion是JetBrains开发的C/C++ IDE，因为它支持cmake，而且支持跨平台开发，所以我就用它了，其他 支持cmake的IDE也是可以的。

![image-20211216234632019](https://tva1.sinaimg.cn/large/008i3skNgy1gxg3vbunmmj30gm0k0myu.jpg)

编辑CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME screenRecorder_win)
project(${PROJECT_NAME} C)
set(CMAKE_C_STANDARD 99)

# 这里目录要替换成你们的电脑中安装资源的目录
# 设置引入头文件所在目录的别名
set(INC_DIR
	D:/Software/ffmpeg/include
	D:/Software/SDL2-2.0.18/x86_64-w64-mingw32/include)
# 设置要引入的库所在目录的别名
set(LINK_DIR
	D:/Software/ffmpeg/lib
	D:/Software/SDL2-2.0.18/x86_64-w64-mingw32/lib)
# 指定引入的头文件所在路径
include_directories(${INC_DIR})
# 指定引入的库文件所在路径
link_directories(${LINK_DIR})
# 构建可执行目标文件所需要的源代码文件
add_executable(recordScreen main.c)
# 指定要链接的库(-l命令)
target_link_libraries(recordScreen mingw32 SDL2main SDL2 avdevice avcodec avformat swscale avutil)
```

因为Windows环境中还用到了动态链接库，因此需要在配置中添加动态链接库的路径

![image-20211216234748572](https://tva1.sinaimg.cn/large/008i3skNgy1gxg3wo467ej317c0g075z.jpg)

![image-20211216234800599](https://tva1.sinaimg.cn/large/008i3skNgy1gxg3wvntl9j31b00u076t.jpg)

> path=D:\Software\ffmpeg\bin\;C:\Software\mingw64\bin\;D:\Software\SDL2-2.0.18\x86_64- w64-mingw32\bin 
>
> 这里需要三个资源的bin目录，要把mingw32的bin目录也添加到路径中，我查了好多资料才发现 之前一直不成功就是因为这个动态链接库目录没有添加到路径中。

### 3. 编译及运行结果

编译后的程序为：screenRecorder_win.exe

可以在`cmake-build-debug`目录下查看到编译出的程序

该程序需要依赖以下动态库，需要在其他环境运行本程序，需要将下列动态库拷贝到程序所在文件夹。

![image-20211217000023601](https://tva1.sinaimg.cn/large/008i3skNgy1gxg49r4f7ej311o0bo3z5.jpg)

## 二、录屏推流程序

该程序使用FFmpeg库和SDL2库，通过gdigrab抓取Windows屏幕，能够实现Windows屏幕的录制，并通过rtmp协议推流到流媒体服务器。

> 通过Nginx搭建流媒体服务器请参考：[在Ubuntu 14 上安装 Nginx-RTMP 流媒体服务器 ](https://www.cnblogs.com/cocoajin/p/4353767.html)

### 1. 使用帮助

通过cmd进入Windows的终端，在相应文件夹下输入命令

`./screenRecorder_win.exe -? ` 或 `./screenRecorder_win.exe --help ` 获取帮助

程序将输出以下帮助

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
> -i|--input         Specifies the input source for the video. The default is 'desktop'.
>
> -?|--help          Show the information

### 2. 程序思路

程序内有详细的注释，请参考注释。程序可以分为以下几个步骤，每个步骤有相应的函数。

> 1. 打开设备(gdigrab)
> 2. 找到视频流的索引
> 3. 打开输入视频的解码器
> 4. 打开输出视频的编码器
> 5. 打开推流的输出上下文
> 6. 开启子线程，刷新SDL播放器
> 7.  录制屏幕，在SDL播放器中实时播放，推流到指定的rtmp服务器

### 3. 程序流程

![windows端流程图](https://tva1.sinaimg.cn/large/008i3skNly1gxh4altdkjj31ck0u078f.jpg)

### 4. 程序演示

![image-20211217111139621](https://tva1.sinaimg.cn/large/008i3skNly1gxgno774ibj31hc0u0wqt.jpg)

### 5. 程序参考内容

1. [最简单的基于FFmpeg的AVDevice例子（屏幕录制）](https://blog.csdn.net/leixiaohua1020/article/details/39706721)
2. [最简单的基于FFmpeg的推流器（以推送RTMP为例）](https://blog.csdn.net/leixiaohua1020/article/details/39803457)

