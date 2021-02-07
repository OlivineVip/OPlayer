#include <jni.h>
#include <string>
#include "NativeQueue.h"

#include "SimplePlayer.h"


extern "C" {
#include <libavutil/avutil.h>
}



extern "C"
JNIEXPORT jstring JNICALL
Java_com_olivine_lib_1audio_AudioNativeManager_stringFromJNI12(JNIEnv *env, jobject thiz) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}extern "C"


JNIEXPORT jstring JNICALL
Java_com_olivine_lib_1audio_AudioNativeManager_ffmpegVersion(JNIEnv *env, jobject thiz) {
    const char *version = av_version_info();
    return env->NewStringUTF(version);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_olivine_lib_1audio_AudioNativeManager_simplePlayer(JNIEnv *env, jobject thiz,
                                                            jstring url) {
    const char *mUrl = env->GetStringUTFChars(url, 0);
    SimplePlayer *player = new SimplePlayer();
    player->player(env, mUrl);
}