/*
 * Copyright (c) 2014 Yrom Wang <http://www.yrom.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.wmr.androidrecordscreen.activity;

import android.Manifest;
import android.app.Activity;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.BitmapFactory;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.os.Build;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.app.NotificationCompat;

import com.wmr.androidrecordscreen.R;
import com.wmr.androidrecordscreen.rtmp.RESFlvData;
import com.wmr.androidrecordscreen.rtmp.RESFlvDataCollecter;
import com.wmr.androidrecordscreen.task.RtmpStreamingSender;
import com.wmr.androidrecordscreen.task.ScreenRecorder;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Modified by raomengyang
 * Modified by WuMingrui 2021-12-15
 */
public class MainActivity extends Activity implements View.OnClickListener {
    // Used to load the 'androidrecordscreen' library on application startup.
    static {
        System.loadLibrary("androidrecordscreen");
    }
    private static final int REQUEST_CODE = 1;
    private Button mButton;
    private EditText mRtmpAddET;
    private MediaProjectionManager mMediaProjectionManager;
    private ScreenRecorder mRecorder;
    private RtmpStreamingSender streamingSender;
    private ExecutorService executorService;
    private boolean isRecording;
    public static final int PENDING_REQUEST_CODE = 0x01;
    private static final int NOTIFICATION_ID = 3;
    NotificationManager mNotificationManager;
    // 确认是否授权
    boolean authorized = false;
    private static final int REQUEST_CODE_ADDRESS = 1;
    private static final String[] PERMISSIONS = {
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mButton = (Button) findViewById(R.id.button);
        mRtmpAddET = (EditText) findViewById(R.id.rtmp_address);
        mButton.setOnClickListener(this);
        mMediaProjectionManager = (MediaProjectionManager) getSystemService(MEDIA_PROJECTION_SERVICE);
        mNotificationManager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        verifyPermissions();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        MediaProjection mediaProjection = mMediaProjectionManager.getMediaProjection(resultCode, data);
        if (mediaProjection == null) {
            Log.e("@@", "media projection is null");
            return;
        }
        String rtmpAddr = mRtmpAddET.getText().toString().trim();
        if (TextUtils.isEmpty(rtmpAddr)) {
            Toast.makeText(this, "rtmp地址不能为空", Toast.LENGTH_SHORT).show();
            return;
        }
        // 开始通知
        initNotification();
        streamingSender = new RtmpStreamingSender();
        streamingSender.sendStart(rtmpAddr);
        RESFlvDataCollecter collecter = new RESFlvDataCollecter() {
            @Override
            public void collect(RESFlvData flvData, int type) {
                streamingSender.sendFood(flvData, type);
            }
        };
        mRecorder = new ScreenRecorder(collecter, RESFlvData.VIDEO_WIDTH, RESFlvData.VIDEO_HEIGHT, RESFlvData.VIDEO_BITRATE, 1, mediaProjection);
        mRecorder.start();
        executorService = Executors.newCachedThreadPool();
        executorService.execute(streamingSender);
        mButton.setText("停止录制屏幕");
        Toast.makeText(this, "正在录制屏幕...", Toast.LENGTH_SHORT).show();
        moveTaskToBack(true);
    }

    @Override
    public void onClick(View v) {
        if (mRecorder != null) {
            mRecorder.quit();
            mRecorder = null;
            if (streamingSender != null) {
                streamingSender.sendStop();
                streamingSender.quit();
                streamingSender = null;
            }
            if (executorService != null) {
                executorService.shutdown();
                executorService = null;
            }
            mButton.setText("重新录制屏幕");
            // 取消通知
            if (mNotificationManager != null) {
                mNotificationManager.cancel(NOTIFICATION_ID);
            }
        } else {
            isRecording = true;
            Intent captureIntent = mMediaProjectionManager.createScreenCaptureIntent();
            startActivityForResult(captureIntent, REQUEST_CODE);
        }
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mRecorder != null) {
            mRecorder.quit();
            mRecorder = null;
            if (streamingSender != null) {
                streamingSender.quit();
                streamingSender = null;
            }
            if (executorService != null) {
                executorService.shutdown();
                executorService = null;
            }
            mButton.setText("重新录制屏幕");
        }
    }


    private void initNotification() {
        NotificationCompat.Builder builder;
        //Android8.0以上的适配
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            //创建通知渠道实例（这三个参数是必须要有的）
            //通知渠道的id
            String channelId = "recorder";
            NotificationChannel channel = new NotificationChannel(channelId, "录屏消息", NotificationManager.IMPORTANCE_HIGH);
            //将实例交给管理器
            mNotificationManager.createNotificationChannel(channel);
            builder = new NotificationCompat.Builder(this, channelId)
                    .setPriority(NotificationCompat.PRIORITY_HIGH)
                    .setSmallIcon(R.drawable.ic_baseline_cast)
                    .setContentTitle(getResources().getString(R.string.app_name))
                    .setLargeIcon(BitmapFactory.decodeResource(getResources(),R.mipmap.ic_record))
                    .setContentText("您正在录制视频内容")
                    .setOngoing(true)
                    .setWhen(System.currentTimeMillis())
                    .setDefaults(Notification.DEFAULT_VIBRATE);
        } else {
            builder = new NotificationCompat.Builder(this)
                    .setSmallIcon(R.drawable.ic_baseline_cast)
                    .setContentTitle(getResources().getString(R.string.app_name))
                    .setContentText("您正在录制视频内容")
                    .setOngoing(true)
                    .setWhen(System.currentTimeMillis())
                    .setDefaults(Notification.DEFAULT_VIBRATE);
        }

        Intent backIntent = new Intent(this, MainActivity.class);
        backIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_SINGLE_TOP);
        PendingIntent pendingIntent = PendingIntent.getActivity(this, PENDING_REQUEST_CODE, backIntent, PendingIntent.FLAG_UPDATE_CURRENT);
        builder.setContentIntent(pendingIntent);
        mNotificationManager.notify(NOTIFICATION_ID, builder.build());
    }

    public void verifyPermissions() {
        // 获取存储的读写权限
//        int SYSTEM_ALERT_WINDOW_permission = ActivityCompat.checkSelfPermission(this, Manifest.permission.SYSTEM_ALERT_WINDOW);
        int WRITE_EXTERNAL_STORAGE_permission = ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE);
        if (WRITE_EXTERNAL_STORAGE_permission != PackageManager.PERMISSION_GRANTED) { //||
//                SYSTEM_ALERT_WINDOW_permission != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    PERMISSIONS,
                    REQUEST_CODE_ADDRESS);
        } else {
            authorized = true;
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_CODE_ADDRESS) {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) { //&&
//                    grantResults[1] == PackageManager.PERMISSION_GRANTED) {
                authorized = true;
            } else {
                Toast.makeText(getBaseContext(), "权限被禁用", Toast.LENGTH_LONG).show();
            }
        }
    }

}
