package com.hejunlin.ffmpegdecoder;

import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class LauncherActivity extends AppCompatActivity implements View.OnClickListener {
    private Button mButton;
    private EditText mEditText;



    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_launcher);
        mButton = (Button) findViewById(R.id.bt_play);
        mEditText = (EditText) findViewById(R.id.rtmp_input);
        mButton.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        String address = mEditText.getText().toString().trim();
        if (TextUtils.isEmpty(address)) {
            Toast.makeText(this, "rtmp地址不能为空", Toast.LENGTH_SHORT).show();
        } else {
            Intent intent = new Intent(this, MainActivity.class);
            intent.putExtra("address", address);
            startActivity(intent);
        }
    }
}
