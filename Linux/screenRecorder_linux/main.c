//
// Created by WMR on 2021/12/10.
//


#include "captureScreen.h"
int thread_exit = 0;
int screen_height = 480;
int screen_width = 640;
char *input = ":0";
char *video_size = NULL;
char *outUrl = "rtmp://110.40.193.165:1935/live/tencent";

/**
 * 刷新SDL播放器的线程
 *
 * @param opaque
 * @return
 */
int sfp_refresh_thread(void *opaque) {
    thread_exit = 0;
    while (!thread_exit) {
        SDL_Event event;
        event.type = SFM_REFRESH_EVENT;
        SDL_PushEvent(&event);
        SDL_Delay(40);
    }
    thread_exit = 0;
    //Break
    SDL_Event event;
    event.type = SFM_BREAK_EVENT;
    SDL_PushEvent(&event);
    return 0;
}


static const struct option long_options[] = {
        {"height", required_argument, NULL, 'h'},
        {"width", required_argument, NULL, 'w'},
        {"address", required_argument, NULL, 'a'},
        {"video_size", required_argument, NULL, 'v'},
        {"input", required_argument, NULL, 'i'},
        {"help", no_argument, NULL, '?'}
};

static void usage(void) {
    fprintf(stderr,
            "screenRecorder [option]... \n"
            "  -h|--height        Specify the height of the output image of the recording screen. The default is 480.\n"
            "  -w|--width         Specify the width of the output image of the recording screen. The default is 640.\n"
            "  -a|--address       Specify the rtmp server address. The default is rtmp://110.40.193.165:1935/live/tencent\n"
            "  -v|--video_size    Specify the resolution of the recorded video. It is automatically set according to the system.\n"
            "  -i|--input         Specifies the input source for the video. The default is ':0'\n"
            "  -?|--help          Show the information.\n");
}


int main(int argc, char *argv[]) {

    int opt = 0;
    int options_index = 0;


    while ((opt = getopt_long(argc, argv, "h:w:a:i:v:?", long_options, &options_index)) != EOF) {
        switch (opt) {
            case 'a':
                outUrl = optarg;
                break;
            case 'h':
                screen_height = atoi(optarg);
                break;
            case 'w':
                screen_width = atoi(optarg);
                break;
            case 'i':
                input = optarg;
                break;
            case 'v':
                video_size = optarg;
                break;
            case '?': usage(); return 2;
        }
    }




    // 设置日志级别
    av_log_set_level(AV_LOG_INFO);

    int ret = 0;
    char errors[1024] = {0,};
    // 视频流索引
    int video_index = 0;
    // 输入的上下文
    AVFormatContext *ifmt_ctx = NULL;
    // 输出的上下文
    AVFormatContext *ofmt_ctx = NULL;
    // 解码相关的结构体
    AVCodecContext *deCodecCtx = NULL;
    AVCodec *deCodec = NULL;
    // 编码器上下文
    AVCodecContext *enCodecCtx = NULL;

    // 1. 打开设备
    ret = open_device(&ifmt_ctx);
    if (ret < 0) {
        goto __ERROR;
    }
    // 2.找到视频流的索引
    video_index = find_video_stream(ifmt_ctx);
    if (video_index < 0) {
        goto __ERROR;
    }
    // 3.打开输入视频的解码器
    ret = open_decoder(ifmt_ctx, video_index, &deCodecCtx, &deCodec);
    if (ret < 0) {
        goto __ERROR;
    }
    // 4. 打开输出视频的编码器
    ret = open_encoder(screen_width, screen_height, &enCodecCtx);
    if (ret < 0) {
        goto __ERROR;
    }

    // 5. 打开推流的输出上下文
    ret = open_output(&ofmt_ctx, ifmt_ctx, enCodecCtx, outUrl, video_index);
    if (ret < 0) {
        goto __ERROR;
    }
    // 6. 开启子线程，刷新SDL播放器
    SDL_Thread *video_tid = SDL_CreateThread(sfp_refresh_thread,NULL);

    // 7. 录制屏幕，在SDL播放器中实时播放，推流到指定的rtmp服务器
    record_screen(ifmt_ctx, ofmt_ctx, deCodecCtx, enCodecCtx, video_index);



__ERROR:
    // 垃圾回收
    if (deCodecCtx) {
        avcodec_close(deCodecCtx);
    }

    if (enCodecCtx) {
        avcodec_close(enCodecCtx);
    }

    // 关闭AVFormatContext
    if (ifmt_ctx) {
        avformat_close_input(&ifmt_ctx);
    }
    if (ofmt_ctx) {
        avformat_free_context(ofmt_ctx);
    }
    return 0;
}
