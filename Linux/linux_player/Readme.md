## Requirement

### 安装ffmpeg

https://blog.csdn.net/qq_38863413/article/details/105017775

### 安装sdl2

```c
sudo apt-get install libsdl2-2.0
sudo apt-get install libsdl2-dev
apt-get install libsdl2-mixer-dev
sudo apt-get install libsdl2-image-dev
sudo apt-get install libsdl2-ttf-dev
sudo apt-get install libsdl2-gfx-dev
```

## 编译

```c
clang -g -o player player.c `pkg-config --cflags --libs sdl2 libavformat libavcodec libswscale`
```

## 播放Rtmp视频流

```c
./player  rtmp://58.200.131.2:1935/livetv/hunantv //视频流地址
```

## 效果演示

![image-20211217191954574](https://tva1.sinaimg.cn/large/008i3skNly1gxh1s84w2wj30i10e4q3v.jpg)