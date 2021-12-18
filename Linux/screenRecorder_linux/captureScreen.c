#include "captureScreen.h"
#include <stdio.h>



// 用来记录frame的pts(Presentation Time Stamp)
int base = 1;

int open_device(AVFormatContext **ifmt_ctx) {
    // 用来展示函数调用结果的ret和返回错误结果的error
    char errors[1024] = {0,};
    int ret = 0;

    AVInputFormat *ifmt = NULL;
    // AVDictionary **options 打开设备的参数，视频需要设置options
    AVDictionary *options = NULL;
    // 设备名称
    char *device_name = NULL;
    // 注册设备
    avdevice_register_all();

    // 初始化网络库
    avformat_network_init();

    // 设备名称
    device_name = input;
    // 获得格式
    ifmt = av_find_input_format("x11grab");
    // 设置options的参数
    // 指定分辨率
    if (video_size != NULL) {
        av_dict_set(&options, "video_size", video_size, 0);
    }
    // 指定帧率
//    av_dict_set(&options, "framerate", "25", 0);

    // 打开设备
    if ((ret = avformat_open_input(ifmt_ctx, device_name, ifmt, &options)) < 0) {
        av_strerror(ret, errors, 1024);
        av_log(NULL, AV_LOG_ERROR, "Failed to open video device, [%d]%s\n", ret, errors);
        return -1;
    }
    return 0;

}

int open_output(AVFormatContext **ofmt_ctx, AVFormatContext *ifmt_ctx, AVCodecContext *enCodecCtx, char* outUrl, int video_index) {
    // 用来展示函数调用结果的ret和返回错误结果的error
    char errors[1024] = {0,};
    int ret = 0;
    ret = avformat_alloc_output_context2(ofmt_ctx, NULL, "flv", outUrl);
    if (ret < 0) {
        av_strerror(ret, errors, 1024);
        av_log(NULL, AV_LOG_ERROR,"Failed to create output context,[%d]%s\n", ret, errors);
        return -1;
    }
    AVOutputFormat *ofmt = NULL;
    ofmt = (*ofmt_ctx)->oformat;

    // 创建输出流
    AVStream *out_scream = avformat_new_stream(*ofmt_ctx, ifmt_ctx->streams[video_index]->codec->codec);
    if (!out_scream) {
        av_log(NULL, AV_LOG_ERROR, "Failed to create output stream.\n");
        return -1;
    }

    // 复制配置信息
    ret = avcodec_copy_context(out_scream->codec, enCodecCtx);
    out_scream->time_base = (AVRational) {1, 25};

    if (ret < 0) {
        av_strerror(ret, errors, 1024);
        av_log(NULL, AV_LOG_ERROR, "Failed to copy configuration information for output stream, [%d]%s\n", ret,
               errors);
        return -1;
    }
    out_scream->codec->codec_tag = 0;

    //Dump Format------------------
    av_dump_format(*ofmt_ctx, 0, outUrl, 1);

    // rtmp推流
    // 打开IO
    ret = avio_open(&((*ofmt_ctx)->pb), outUrl, AVIO_FLAG_WRITE);
    if (ret < 0) {
        av_strerror(ret, errors, 1024);
        av_log(NULL, AV_LOG_ERROR,"Failed to open IO for output,[%d]%s\n", ret, errors);
        return -1;
    }

    //写入头信息
    ret = avformat_write_header(*ofmt_ctx, 0);
    if (ret < 0)
    {
        av_strerror(ret, errors, 1024);
        av_log(NULL, AV_LOG_ERROR,"Failed to write header for output,[%d]%s\n", ret, errors);
        return -1;
    }
    return 0;
}

int find_video_stream(AVFormatContext *ifmt_ctx) {
    int video_index = -1;
    // 用来展示函数调用结果的ret和返回错误结果的error
    char errors[1024] = {0,};
    int ret = 0;
    // 获取流信息
    if ((ret = avformat_find_stream_info(ifmt_ctx, NULL) < 0)) {
        av_strerror(ret, errors, 1024);
        av_log(NULL, AV_LOG_ERROR, "Couldn't find stream information, [%d]%s\n", ret, errors);
        return -1;
    }


    for (int i = 0; i < ifmt_ctx->nb_streams; i++) {
        if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
            break;
        }
    }
    if (video_index == -1) {
        av_log(NULL, AV_LOG_ERROR, "Didn't find a video stream.\n");
        return -1;
    }
    //Dump Format------------------
    av_dump_format(ifmt_ctx, 0, "desktop", 0);
    return video_index;
}

int open_decoder(AVFormatContext *ifmt_ctx,
                 int video_index,
                 AVCodecContext **deCodecCtx,
                 AVCodec **deCodec) {

    *deCodecCtx = ifmt_ctx->streams[video_index]->codec;
    *deCodec = avcodec_find_decoder((*deCodecCtx)->codec_id);

    if (*deCodec == NULL) {
        av_log(NULL, AV_LOG_ERROR, "Could not find codec.\n");
        return -1;
    }

    if (avcodec_open2(*deCodecCtx, *deCodec, NULL) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not open codec.\n");
        return -1;
    }
    return 0;
}


int open_encoder(int width,
                 int height,
                 AVCodecContext **enc_ctx) {
    int ret = 0;
    char errors[1024] = {0,};

    AVCodec *codec = NULL;
    // 根据名字找到对应的编码器
    codec = (AVCodec *) avcodec_find_encoder_by_name("libx264");
    if (!codec) {
        av_log(NULL, AV_LOG_ERROR, "Codec libx264 not found.\n");
        return -1;
    }
    // 分配codec上下文
    *enc_ctx = avcodec_alloc_context3(codec);
    if (!*enc_ctx) {
        av_log(NULL, AV_LOG_ERROR, "Could not allocate enc_ctx.\n");
        return -1;
    }

    // SPS/PPS
    // 设置profile，表示压缩参数
    (*enc_ctx)->profile = FF_PROFILE_H264_HIGH_444;
    // 设置level，表示视频参数
    (*enc_ctx)->level = 50; // 表示LEVEL是5.0

    // 设置分辨率
    (*enc_ctx)->width = width; // 640
    (*enc_ctx)->height = height; // 480

    // 设置GOP
    (*enc_ctx)->gop_size = 250;
    (*enc_ctx)->keyint_min = 25; // 多少帧之后，就会插入I帧 optional

    // 设置B帧数据
    (*enc_ctx)->max_b_frames = 3; // optional
    (*enc_ctx)->has_b_frames = 1; // optional

    // 参考帧的数量
    (*enc_ctx)->refs = 3;   // optional

    // 像素格式，设置输入YUV格式
    (*enc_ctx)->pix_fmt = AV_PIX_FMT_YUV420P;

    // 设置码率
    (*enc_ctx)->bit_rate = 600000; // 600kbps

    // 设置帧率
    (*enc_ctx)->time_base = (AVRational) {1, 25}; // 帧与帧之间的间隔是timebase
    (*enc_ctx)->framerate = (AVRational) {25, 1}; // 帧率，每秒25帧


    (*enc_ctx)->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    // Set H264 preset and tune
    AVDictionary *param = 0;
    av_dict_set(&param, "preset", "ultrafast", 0);
    av_dict_set(&param, "tune", "zerolatency", 0);

    // 打开编码器
    if ((ret = avcodec_open2(*enc_ctx, codec, &param)) < 0) {
        av_strerror(ret, errors, 1024);
        av_log(NULL, AV_LOG_ERROR, "Failed to open encoder, [%d]%s\n", ret, errors);
        return -1;
    }
    return 0;
}

int init_SDL(int screen_w,
             int screen_h,
             SDL_Surface **screen,
             SDL_Overlay **bmp,
             SDL_Rect *rect) {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        printf( "Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }

    *screen = SDL_SetVideoMode(screen_w, screen_h, 0,0);

    if(!*screen) {
        printf("SDL: could not set video mode - exiting:%s\n",SDL_GetError());
        return -1;
    }

    *bmp = SDL_CreateYUVOverlay(screen_w, screen_h, SDL_YV12_OVERLAY, *screen);
    if (!*bmp) {
        printf("SDL: could not create yuv overlay - exiting:%s\n",SDL_GetError());
        return -1;
    }
    rect->x = 0;
    rect->y = 0;
    rect->w = screen_w;
    rect->h = screen_h;

    SDL_WM_SetCaption("Simplest FFmpeg Grab Desktop",NULL);

    return 0;
}

void encode(AVCodecContext *enc_ctx,
            AVFrame *frame,
            AVPacket *new_packet,
            AVFormatContext *ifmt_ctx,
            AVFormatContext *ofmt_ctx,
            int64_t start_time,
            int video_index) {
    int ret = 0;
    char errors[1024] = {0,};

    AVStream *in_stream, *out_stream;

    // 送原始数据给编码器进行编码
    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        } else {
            av_strerror(ret, errors, 1024);
            printf("Error, Failed to send a frame for encoding!, [%d]%s\n", ret, errors);
            exit(1);
        }
    }

    // 从编码器获取编码好的数据
    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, new_packet);
        // 如果编码器数据不足时会返回  EAGAIN，或者到数据尾时会返回 AVERROR_EOF
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        } else if (ret < 0) {
            printf("Error, Failed to encode!/n");
            exit(1);
        }


        //FIX：No PTS (Example: Raw H.264)
        //Simple Write PTS
        if (new_packet->pts == AV_NOPTS_VALUE) {
            //Write PTS
            AVRational time_base1 = ifmt_ctx->streams[video_index]->time_base;
            //Duration between 2 frames (us)
            int64_t calc_duration =
                    (double) AV_TIME_BASE / av_q2d(ifmt_ctx->streams[video_index]->r_frame_rate);
            //Parameters
            new_packet->pts =
                    (double) (frame->pts * calc_duration) / (double) (av_q2d(time_base1) * AV_TIME_BASE);
            new_packet->dts = new_packet->pts;
            new_packet->duration =
                    (double) calc_duration / (double) (av_q2d(time_base1) * AV_TIME_BASE);
        }

        //Important:Delay
        if (new_packet->stream_index == video_index) {
            AVRational time_base = ifmt_ctx->streams[video_index]->time_base;
            AVRational time_base_q = {1, AV_TIME_BASE};
            int64_t pts_time = av_rescale_q(new_packet->dts, time_base, time_base_q);
            int64_t now_time = av_gettime() - start_time;
            if (pts_time > now_time)
                av_usleep(pts_time - now_time);
        }

        in_stream  = ifmt_ctx->streams[new_packet->stream_index];
        out_stream = ofmt_ctx->streams[new_packet->stream_index];

        if(new_packet->stream_index==video_index){
//            printf("Send %lld video frames to output URL\n", frame->pts - 1);
        }

// _______________________________________________________

//        new_packet->stream_index = out_stream->index;

        //Write PTS
        AVRational time_base = ofmt_ctx->streams[0]->time_base;//{ 1, 1000 };
        AVRational r_framerate1 = {50, 2 };//{ 50, 2 };
        AVRational time_base_q = { 1, AV_TIME_BASE };

        //Duration between 2 frames (us)
        int64_t calc_duration = (double)(AV_TIME_BASE)*(1 / av_q2d(r_framerate1));	//内部时间戳
        //Parameters
        //enc_pkt.pts = (double)(framecnt*calc_duration)*(double)(av_q2d(time_base_q)) / (double)(av_q2d(time_base));

        new_packet->pts = av_rescale_q(base*calc_duration, time_base_q, time_base);
        new_packet->dts = new_packet->pts;
        new_packet->duration = av_rescale_q(calc_duration, time_base_q, time_base); //(double)(calc_duration)*(double)(av_q2d(time_base_q)) / (double)(av_q2d(time_base));
        new_packet->pos = -1;

        //Delay
        int64_t pts_time = av_rescale_q(new_packet->dts, time_base, time_base_q);
        int64_t now_time = av_gettime() - start_time;
        if (pts_time > now_time)
            av_usleep(pts_time - now_time);

        //推送帧数据
        ret = av_interleaved_write_frame(ofmt_ctx, new_packet);
        if (ret < 0) {
            printf("Error muxing packet\n");
            break;
        }
    }

}


void record_screen(AVFormatContext *ifmt_ctx,
                   AVFormatContext *ofmt_ctx,
                   AVCodecContext *deCodecCtx,
                   AVCodecContext *enCodecCtx,
                   int video_index) {
    // 用来展示函数调用结果的ret和返回错误结果的error
    char errors[1024] = {0,};
    int ret = 0;

    // SDL相关参数
    SDL_Surface *screen = NULL;
    SDL_Overlay *bmp = NULL;
    SDL_Rect rect;
    //Event Loop
    SDL_Event event;


    // 主要用于处理图片像素数据, 可以完成图片像素格式的转换, 图片的拉伸等工作
    struct SwsContext *img_convert_ctx;

    AVFrame *frame, *new_frame;
    AVPacket *packet, *new_packet;

    int got_picture;


    // 保存编码后的数据
    packet = av_packet_alloc();
    new_packet = av_packet_alloc();

    // frame是从设备中读取的AVPacket解析后的，new_frame是转换后的，要输给SDL和编码的
    frame = av_frame_alloc();
    new_frame = av_frame_alloc();
    new_frame->width = screen_width;
    new_frame->height = screen_height;
    new_frame->format = AV_PIX_FMT_YUV420P;

    // 初始化SDL, 并返回渲染器和纹理
    ret = init_SDL(screen_width, screen_height, &screen, &bmp, &rect);
    if (ret < 0) {
        goto __ERROR;
    }


    img_convert_ctx = sws_getContext(deCodecCtx->width,
                                     deCodecCtx->height,
                                     deCodecCtx->pix_fmt,
                                     screen_width,
                                     screen_height,
                                     AV_PIX_FMT_YUV420P,
                                     SWS_FAST_BILINEAR,
                                     NULL,
                                     NULL,
                                     NULL);

    int linesize[4];
    uint8_t *data[4];
    av_image_alloc(data, linesize, new_frame->width, new_frame->height, AV_PIX_FMT_YUV420P, 1);

    int64_t start_time = av_gettime();
    for (;;) {
        //Wait
        SDL_WaitEvent(&event);
        if(event.type==SFM_REFRESH_EVENT){
            //------------------------------
            if(av_read_frame(ifmt_ctx, packet)>=0){
                if(packet->stream_index==video_index){
                    ret = avcodec_decode_video2(deCodecCtx, frame, &got_picture, packet);
                    if(ret < 0){
                        printf("Decode Error.\n");
                        goto __ERROR;
                    }
                    if(got_picture){
                        SDL_LockYUVOverlay(bmp);
                        new_frame->data[0]=bmp->pixels[0];
                        new_frame->data[1]=bmp->pixels[2];
                        new_frame->data[2]=bmp->pixels[1];
                        new_frame->linesize[0]=bmp->pitches[0];
                        new_frame->linesize[1]=bmp->pitches[2];
                        new_frame->linesize[2]=bmp->pitches[1];
                        sws_scale(img_convert_ctx, (const unsigned char* const*)frame->data, frame->linesize, 0, deCodecCtx->height, new_frame->data, new_frame->linesize);

                        new_frame->pts = base++;

                        encode(enCodecCtx, new_frame, new_packet, ifmt_ctx, ofmt_ctx, start_time, video_index);

                        SDL_UnlockYUVOverlay(bmp);

                        SDL_DisplayYUVOverlay(bmp, &rect);

                    }
                }
                av_free_packet(packet);
            }else{
                //Exit Thread
                thread_exit=1;
            }
        }else if(event.type==SDL_QUIT){
            thread_exit=1;
            // 防止丢帧
            encode(enCodecCtx, NULL, new_packet, ifmt_ctx, ofmt_ctx, start_time, video_index);
        }else if(event.type==SFM_BREAK_EVENT){
            break;
        }

    }
    //写文件尾（Write file trailer）
    av_write_trailer(ofmt_ctx);

    __ERROR:
    // 垃圾回收

    if (frame) {
        av_frame_free(&frame);
    }
    if (new_frame) {
        av_frame_free(&new_frame);
    }
    if (packet) {
        av_packet_free(&packet);
    }
    if (new_packet) {
        av_packet_free(&new_packet);
    }

    if (screen) {
        SDL_FreeSurface(screen);
    }

    if (bmp) {
        SDL_FreeYUVOverlay(bmp);
    }

    if (img_convert_ctx) {
        sws_freeContext(img_convert_ctx);
    }
    SDL_Quit();

}
