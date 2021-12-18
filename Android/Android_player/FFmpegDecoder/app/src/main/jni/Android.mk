LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := avcodec
LOCAL_SRC_FILES := prebuilt/libavcodec-57.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avformat
LOCAL_SRC_FILES := prebuilt/libavformat-57.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avutil
LOCAL_SRC_FILES := prebuilt/libavutil-55.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swresample
LOCAL_SRC_FILES := prebuilt/libswresample-2.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swscale
LOCAL_SRC_FILES := prebuilt/libswscale-4.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := yuiopffmpeg.c
LOCAL_LDLIBS += -llog -lz -landroid
LOCAL_MODULE := yuiopffmpeg
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

LOCAL_SHARED_LIBRARIES:= avcodec avformat avutil swresample swscale

include $(BUILD_SHARED_LIBRARY)