package com.olivine.oplayer

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.olivine.lib_audio.AudioNativeManager
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    private val SOURCE_LIVE = "http://livestream.1766.today:1768/live1.mp3"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        tv_content.text = AudioNativeManager().ffmpegVersion()

        tv_content.setOnClickListener {
            Thread{
                Log.d("TAG", "onCreate: $SOURCE_LIVE")
                AudioNativeManager().simplePlayer(SOURCE_LIVE)
            }.start()

        }
    }
}