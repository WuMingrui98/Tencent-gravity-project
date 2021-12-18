package com.hejunlin.ffmpegdecoder;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

//继承appcompatactivity的父类，实现SurfaceHolder.Callback的接口
public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback{
    private String url = "rtmp://110.40.193.165:1935/live/tencent";

    private SurfaceHolder mSurfaceHolder;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        url = getIntent().getStringExtra("address");
        SurfaceView surfaceView = (SurfaceView) findViewById(R.id.surface_view);
        mSurfaceHolder = surfaceView.getHolder();
        mSurfaceHolder.addCallback(this);
    }
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_main);
//        TextView textView = (TextView) findViewById(R.id.text);
//        textView.setText(new JNIUtils().stringFromJNI());
//    }
//}


    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        new Thread(new Runnable() {
            @Override
            public void run() {

//                String url = " rtmp://184.72.239.149/void/mp4://BigBuck-Bunny _175k.mov";
//                String url = "/Users/hzp/Desktop/out.h264";
//                String url = '/Users/hzp/Desktop/out.h264';
                NativePlayer.playVideo(url, mSurfaceHolder.getSurface());
            }
        }).start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }
}
