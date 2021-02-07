package com.olivine.lib_audio

class AudioNativeManager {

    companion object {
        init {
            System.loadLibrary("audio-lib")
        }
    }


    external fun simplePlayer(url: String)


    external fun ffmpegVersion(): String

}