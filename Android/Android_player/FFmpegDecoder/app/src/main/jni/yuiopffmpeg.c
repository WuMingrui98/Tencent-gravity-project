#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "com_hejunlin_ffmpegdecoder_NativePlayer.h"
#include "log.h"

JNIEXPORT jint JNICALL Java_com_hejunlin_ffmpegdecoder_NativePlayer_playVideo
        (JNIEnv * env, jclass clazz, jstring url, jobject surface)
{
    LOGD("start playvideo... url");

    // 视频url
    const char * file_name = (*env)->GetStringUTFChars(env, url, JNI_FALSE);
    LOGD("start playvideo... url, %s", file_name);
    av_register_all();
    //avformat_network_init();


    AVFormatContext * pFormatCtx = avformat_alloc_context();


    // Open video file av_open_input_file
    //if(av_open_input_file(&pFormatCtx, file_name, NULL, NULL)!=0) {
    //return avformat_open_input(&pFormatCtx, file_name, NULL, NULL)
    if(avformat_open_input(&pFormatCtx, file_name, NULL, NULL)!=0) {

        LOGE("Couldn't open file:%s\n", file_name);
        return -1; // Couldn't open file
    }

    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL)<0) {
        LOGE("Couldn't find stream information.");
        return -1;
    }

    // Find the first video stream
    int videoStream = -1, i;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO
           && videoStream < 0) {
            videoStream = i;
        }
    }
    if(videoStream==-1) {
        LOGE("Didn't find a video stream.");
        return -1; // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    AVCodecContext  * pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    AVCodec * pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL) {
        LOGE("Codec not found.");
        return -1; // Codec not found
    }

    if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Could not open codec.");
        return -1; // Could not open codec
    }

    // 获取native window
    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, surface);

    // 获取视频宽高
    int videoWidth = pCodecCtx->width;
    int videoHeight = pCodecCtx->height;

    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow,  videoWidth, videoHeight, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;

    if(avcodec_open2(pCodecCtx, pCodec, NULL)<0) {
        LOGE("Could not open codec.");
        return -1; // Could not open codec
    }

    // Allocate video frame
    AVFrame * pFrame = av_frame_alloc();

    // 用于渲染
    AVFrame * pFrameRGBA = av_frame_alloc();
    if(pFrameRGBA == NULL || pFrame == NULL) {
        LOGE("Could not allocate video frame.");
        return -1;
    }

    // Determine required buffer size and allocate buffer
    // buffer中数据就是用于渲染的,且格式为RGBA
    int numBytes=av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height, 1);
    uint8_t * buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCodecCtx->width, pCodecCtx->height, 1);

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,
                             pCodecCtx->height,
                             pCodecCtx->pix_fmt,
                             pCodecCtx->width,
                             pCodecCtx->height,
                             AV_PIX_FMT_RGBA,
                             SWS_BILINEAR,
                             NULL,
                             NULL,
                             NULL);

    int frameFinished;
    AVPacket packet;
    while(av_read_frame(pFormatCtx, &packet)>=0) {
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStream) {

            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // 并不是decode一次就可解码出一帧
            if (frameFinished) {

                // lock native window buffer
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);

                // 格式转换
                sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);

                // 获取stride
                uint8_t * dst = windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t * src = (uint8_t*) (pFrameRGBA->data[0]);
                int srcStride = pFrameRGBA->linesize[0];

                // 由于window的stride和帧的stride不同,因此需要逐行复制
                int h;
                for (h = 0; h < videoHeight; h++) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }

                ANativeWindow_unlockAndPost(nativeWindow);
            }

        }
        av_packet_unref(&packet);
    }

    av_free(buffer);
    av_free(pFrameRGBA);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codecs
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);
    return 0;
}
