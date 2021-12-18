#include <stdio.h>
#include <SDL.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

// compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

int main(int argc, char *argv[]) {

  int ret = -1;

  AVFormatContext *pFormatCtx = NULL; //for opening multi-media file解复用多媒体文件

  int             i, videoStream;

  AVCodecContext  *pCodecCtxOrig = NULL; //codec context编解码上下文
  AVCodecContext  *pCodecCtx = NULL;

  struct SwsContext *sws_ctx = NULL;//图像裁剪上下文

  AVCodec         *pCodec = NULL; // the codecer
  AVFrame         *pFrame = NULL;
  AVPacket        packet;

  int             frameFinished;
  float           aspect_ratio;

  AVPicture  	  *pict  = NULL;//解码是yuv数据，在这里解出来是AVPicture数据，存放yuv的

  SDL_Rect        rect;//渲染区域的大小
  Uint32 	      pixformat;

  //for render
  SDL_Window 	  *win = NULL;
  SDL_Renderer    *renderer = NULL;
  SDL_Texture     *texture = NULL;

  //set defualt size of window 
  int w_width = 640;
  int w_height = 480;

  if(argc < 2) {
    //fprintf(stderr, "Usage: command <file>\n");
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Usage: command <file>");
    return ret;
  }
//SDL的初始化，包括音频、视频和timer
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
    //fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize SDL - %s\n", SDL_GetError());
    return ret;
  }

  //Register all formats and codecs
  //ffmpeg的所有文件格式及编解码的注册
  av_register_all();

  // Open video file，打开video文件，argv是输入的多媒体文件，最终输出是pFormatCtx，输出一个上下文
  if(avformat_open_input(&pFormatCtx, argv[1], NULL, NULL)!=0){
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open video file!");
    goto __FAIL; // Couldn't open file
  }
  
  // Retrieve stream information，查看上下文是否有流信息
  if(avformat_find_stream_info(pFormatCtx, NULL)<0){
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to find stream infomation!");
    goto __FAIL; // Couldn't find stream information
  }
  
  // Dump information about file onto standard error，上下文有流信息的话，将pFormat dump出来
  //音频信息是什么 视频信息是什么，打印多媒体文件信息
  av_dump_format(pFormatCtx, 0, argv[1], 0);
  
  // Find the first video stream
  videoStream=-1;
  //x先找到视频流的id是多少
  //紫色的内容不是`定义的，也不是使用的
  for(i=0; i<pFormatCtx->nb_streams; i++) {
    if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
      videoStream=i;
      break;
    }
  }

  if(videoStream==-1){
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Din't find a video stream!");
    goto __FAIL;// Didn't find a video stream
  }
  
  // Get a pointer to the codec context for the video stream，拿到编解码的上下文
  pCodecCtxOrig=pFormatCtx->streams[videoStream]->codec;

  // Find the decoder for the video stream，找到解码器
  pCodec=avcodec_find_decoder(pCodecCtxOrig->codec_id);
  if(pCodec==NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unsupported codec!\n");
    goto __FAIL; // Codec not found
  }

  // Copy context，将解码器copy一份到pCodecCtx
  pCodecCtx = avcodec_alloc_context3(pCodec);
  if(avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Couldn't copy codec context");
    goto __FAIL;// Error copying codec context
  }

  // Open codec
  if(avcodec_open2(pCodecCtx, pCodec, NULL)<0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open decoder!\n");
    goto __FAIL; // Could not open codec
  }
  
  // Allocate video frame，解码后的数据放在pFrame中
  pFrame=av_frame_alloc();

  //SDL相关的东西
  w_width = pCodecCtx->width;
  w_height = pCodecCtx->height;

  win = SDL_CreateWindow( "Media Player",
		          SDL_WINDOWPOS_UNDEFINED,
		  	  SDL_WINDOWPOS_UNDEFINED,
			  w_width, w_height,
			  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);	  
  if(!win){
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window by SDL");  
    goto __FAIL;
  }
  //创建渲染器
  renderer = SDL_CreateRenderer(win, -1, 0);
  if(!renderer){
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create Renderer by SDL");  
    goto __FAIL;
  }
  //平面模式: Y + U + V (3 planes)
  pixformat = SDL_PIXELFORMAT_IYUV;//纹理里面存放的是yuv格式的数据

    //根据渲染器创建纹理
  texture = SDL_CreateTexture(renderer,
		    pixformat, 
		    SDL_TEXTUREACCESS_STREAMING,
		    w_width, 
		    w_height);

  // initialize SWS context for software scaling，对图像进行裁剪，原始的图像大小是多少，目标的图像大小是多少
  sws_ctx = sws_getContext(pCodecCtx->width,
			   pCodecCtx->height,
			   pCodecCtx->pix_fmt,
			   pCodecCtx->width,
			   pCodecCtx->height,
			   AV_PIX_FMT_YUV420P,
			   SWS_BILINEAR,
			   NULL,
			   NULL,
			   NULL
			   );

  pict = (AVPicture*)malloc(sizeof(AVPicture));
  avpicture_alloc(pict, 
		  AV_PIX_FMT_YUV420P, 
		  pCodecCtx->width, 
		  pCodecCtx->height);


  // Read frames and save first five frames to disk
  //打开多媒体文件，packet是压缩前的数据
  while(av_read_frame(pFormatCtx, &packet)>=0) {
    // Is this a packet from the video stream?
    if(packet.stream_index==videoStream) {
      // Decode video frame，解码视频
      avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
      
      // Did we get a video frame?
      //如果解码成功,判断解码状态
      if(frameFinished) {

	// Convert the image into YUV format that SDL uses，数据最终放在
	sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
		  pFrame->linesize, 0, pCodecCtx->height,
		  pict->data, pict->linesize);//数据存放在pict里面，yuv的封装格式

	SDL_UpdateYUVTexture(texture, NULL, 
			     pict->data[0], pict->linesize[0],
			     pict->data[1], pict->linesize[1],
			     pict->data[2], pict->linesize[2]);
	
	// Set Size of Window
	rect.x = 0;
	rect.y = 0;
	rect.w = pCodecCtx->width;
	rect.h = pCodecCtx->height;

	SDL_RenderClear(renderer);//刷一下屏幕
	SDL_RenderCopy(renderer, texture, NULL, &rect);//纹理拷贝渲染器里面去
	SDL_RenderPresent(renderer);//最终展示出来

      }
    }
    
    // Free the packet that was allocated by av_read_frame
    av_free_packet(&packet);

//    SDL_Event event;
//      for(;;) {
//
//        SDL_WaitEvent(&event);
//        switch(event.type) {
//        case FF_QUIT_EVENT:
//        case SDL_QUIT:
//          SDL_Quit();
//          return 0;
//          goto __QUIT;
//          break;
//        case FF_REFRESH_EVENT:
//          video_refresh_timer(event.user.data1);
//          break;
//        default:
//          break;
//        }
//      }
    //SDL事件，事件队列
    SDL_Event event;
    SDL_PollEvent(&event);//等待事件
    switch(event.type) {
    case SDL_QUIT:
      goto __QUIT;
      break;
      default:
      break;
    }
  }
__QUIT:
  ret = 0;
  
__FAIL:
  // Free the YUV frame
  if(pFrame){
    av_frame_free(&pFrame);
  }
  
  // Close the codec
  if(pCodecCtx){
    avcodec_close(pCodecCtx);
  }

  if(pCodecCtxOrig){
    avcodec_close(pCodecCtxOrig);
  }
  
  // Close the video file
  if(pFormatCtx){
    avformat_close_input(&pFormatCtx);
  }

  if(pict){
    avpicture_free(pict);
    free(pict);
  }

  if(win){
    SDL_DestroyWindow(win);
  }

  if(renderer){
    SDL_DestroyRenderer(renderer);
  }

  if(texture){
    SDL_DestroyTexture(texture);
  }

  SDL_Quit();
  
  return ret;
}
