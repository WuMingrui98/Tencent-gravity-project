package com.hejunlin.ffmpegdecoder;

/**
 */

public class NativePlayer {

    static {
        System.loadLibrary("yuiopffmpeg");
    }

    public static native int playVideo(String url, Object surface);
}
