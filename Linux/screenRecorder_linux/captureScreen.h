#ifndef CAPTURESCREEN_CAPTURESCREEN_H
#define CAPTURESCREEN_CAPTURESCREEN_H


#include <stdio.h>
#include <getopt.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include "SDL/SDL.h"

//Refresh Event
#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)
// Break Event
#define SFM_BREAK_EVENT  (SDL_USEREVENT + 2)


extern int thread_exit;
extern int screen_height;
extern int screen_width;
extern char *input;
extern char *video_size;
//extern char *outUrl;
/**
 * 打开编码器，并设置AVCodecContext
 *
 * @param width 设置分辨率的宽
 * @param height 设置分辨率的高
 * @param enc_ctx 传入指向需要设置的AVCodecContext指针的指针
 * @return 返回-1表示失败，返回0表示成功
 */
int open_encoder(int width, int height, AVCodecContext **enc_ctx);



/**
 * 打开设备
 *
 * @param[out] ifmt_ctx 指向AVFormatContext的指针，需要作为输出参数
 * @return 返回-1表示失败，返回0表示成功
 */
int open_device(AVFormatContext **ifmt_ctx);

/**
 * 打开推流的输出上下文
 *
 * @param ofmt_ctx[out] 输出上下文
 * @param ifmt_ctx 输入上下文
 * @param enCodecCtx 编码器上下文
 * @param outUrl 推流的rtmp地址
 * @param video_index 视频流的索引
 * @return 返回-1表示失败，返回0表示成功
 */
int open_output(AVFormatContext **ofmt_ctx, AVFormatContext *ifmt_ctx, AVCodecContext *enCodecCtx, char* outUrl, int video_index);

/**
 * 找到视频流的索引
 *
 * @param ifmt_ctx
 * @return
 */
int find_video_stream(AVFormatContext *ifmt_ctx);


/**
 * 打开输入设备对应的解码器
 *
 * @param ifmt_ctx AVFormatContext
 * @param video_index 视频流索引
 * @param[out] deCodecCtx 输出解码器上下文
 * @param[out] deCodec 输出解码器
 * @return 返回-1表示失败，返回0表示成功
 */
int open_decoder(AVFormatContext *ifmt_ctx, int video_index, AVCodecContext **deCodecCtx, AVCodec **deCodec);


/**
 * 初始化SDL
 *
 * @param screen_w 屏幕宽度
 * @param screen_h 屏幕高度
 * @param[out] screen
 * @param[out] bmp
 * @param[out] rect
 * @return 返回-1表示失败，返回0表示成功
 */
int init_SDL(int screen_w, int screen_h, SDL_Surface **screen, SDL_Overlay **bmp, SDL_Rect *rect);

/**
 *
 * 对获取的帧进行编码, 并对编码后的帧进行推流
 *
 * @param enc_ctx 编码器上下文
 * @param frame 输入编码器的AVFrame
 * @param newpkt 输出编码器的AVPacket
 * @param ifmt_ctx 用于输入的AVFormatContext
 * @param ofmt_ctx 用于输出的AVFormatContext
 * @param start_time 录屏的开始时间，在函数中需要设置休眠时间，防止推流过快，服务器无法处理
 * @param video_index 视频流的索引
 */
void encode(AVCodecContext *enc_ctx,
            AVFrame *frame,
            AVPacket *new_packet,
            AVFormatContext *ifmt_ctx,
            AVFormatContext *ofmt_ctx,
            int64_t start_time,
            int video_index);

/**
 * 完成录制屏幕，在SDL播放器中实时播放，并编程成H264格式保存
 *
 * @param ifmt_ctx AVFormatContext
 * @param deCodecCtx 输入视频解码器
 * @param enCodecCtx 输出视频编码器
 * @param video_index 视频流的索引
 */
void record_screen(AVFormatContext *ifmt_ctx, AVFormatContext *ofmt_ctx, AVCodecContext *deCodecCtx,
                   AVCodecContext *enCodecCtx, int video_index);

#endif //CAPTURESCREEN_CAPTURESCREEN_H