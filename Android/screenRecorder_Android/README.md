# 安卓平台下实现录屏并推流

本demo参考了作者Yrom [ScreenRecorder](https://github.com/yrom/ScreenRecorder) 和作者myrao [ScreenRecorder](https://github.com/myrao/ScreenRecorder) 的两个项目，通过对两位作者开源的程序学习，结合课程项目的需求，修改出了本demo，感谢两位作者的无私奉献。

本demo在myrao [ScreenRecorder](https://github.com/myrao/ScreenRecorder) 项目的基础上，做了以下修改。

1. 安卓高版本的适配，适配安卓8.0以上版本。
2. 由于本人课程对于demo功能的需求只是对屏幕进行录制并推流，因此对项目中音频录制、相机录制并推流的功能，以及悬浮窗的功能做了削减。本demo的功能要更简单，可能更方便初学者交流学习。
3. 对原项目的通知逻辑进行了修改，当开启录屏推流功能时，开启通知，并常驻通知栏，可以通过通知栏上的通知返回原录屏页面，关闭录屏功能后，取消通知。

## 1. 实现功能

1. 完成Android 录屏功能

2. 采用myrao在 [librestreaming](https://github.com/lakeinchina/librestreaming)基础上修改后的代码，实现FLV格式的分装与rtmp推流

3. 开启录屏推流功能时，开启通知，并常驻通知栏，通过通知栏上的通知返回原录屏页面，关闭录屏功能后，取消通知

4. 在腾讯云搭建流媒体服务器 测试地址 rtmp://110.40.193.165:1935/live/tencent

   > 通过Nginx搭建流媒体服务器请参考：[在Ubuntu 14 上安装 Nginx-RTMP 流媒体服务器 ](https://www.cnblogs.com/cocoajin/p/4353767.html)

## 2. 功能演示

测试平台：HUAWEI Nova 7 Pro (Harmony OS 2.0.0)

<img src="https://tva1.sinaimg.cn/large/008i3skNly1gxgmvx3y0gj30u01t075k.jpg" alt="1201639706072_.pic" style="zoom:30%;" />

<img src="https://tva1.sinaimg.cn/large/008i3skNly1gxgmweklhij30u01t07a1.jpg" alt="image-20211217104456390" style="zoom:30%;" />

<img src="/Users/mingruiwu/Library/Application Support/typora-user-images/image-20211217104526819.png" alt="image-20211217104526819" style="zoom:30%;" />

<img src="https://tva1.sinaimg.cn/large/008i3skNly1gxgmx63nn5j30u01t0tam.jpg" alt="image-20211217104542583" style="zoom:30%;" />

<img src="https://tva1.sinaimg.cn/large/008i3skNly1gxgmvb2d6vj30u00u0adr.jpg" alt="IMG_20211217_104227" style="zoom: 30%;" />

## 3. 程序思路

1. VirturalDisplay将屏幕渲染到MediaCodec创建的Surface上(Android API)

   * 获取屏幕图像的流程

     ![安卓流程图](https://tva1.sinaimg.cn/large/008i3skNly1gxh1pwj7a9j30ti0m0gmv.jpg)

2. 调用MediaCodec将渲染画面编码为H264裸流(Android API)

3. 将裸流封装成FLV格式(Java)

4. 按照RTMP协议通过librtmp库将视频流推送到RTMP服务器

## 4. 程序流程

![安卓流程图 (1)](https://tva1.sinaimg.cn/large/008i3skNly1gxh1ok2w71j31550u0q6e.jpg)

## 5. 程序参考内容

1. [实现录屏直播（三）MediaProjection + VirtualDisplay + librtmp + MediaCodec实现视频编码并推流到rtmp服务器](https://blog.csdn.net/zxccxzzxz/article/details/55230272)
2. [最简单的基于librtmp的示例：发布（FLV通过RTMP发布）](https://blog.csdn.net/leixiaohua1020/article/details/42104945)
3. [lakeinchina/librestreaming: Android real-time effect filter rtmp streaming library](https://github.com/lakeinchina/librestreaming)
4. [yrom/ScreenRecorder: Implement screen capture without root on Android 5.0+ by using MediaProjectionManager, VirtualDisplay, AudioRecord, MediaCodec and MediaMuxer APIs ](https://github.com/yrom/ScreenRecorder)