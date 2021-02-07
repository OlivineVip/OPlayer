//
// Created by Administrator on 2021/2/5.
//

#ifndef OPLAYER_SIMPLEPLAYER_H
#define OPLAYER_SIMPLEPLAYER_H

#include <android/log.h>
#include <jni.h>

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#define AUDIO_SAMPLE_RATE 44100

extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

class SimplePlayer {

public:
    SimplePlayer();

    ~SimplePlayer();

    void player(JNIEnv *env,const char *url);

};


#endif //OPLAYER_SIMPLEPLAYER_H
