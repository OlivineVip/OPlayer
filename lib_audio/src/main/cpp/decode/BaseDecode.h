//
// Created by Administrator on 2021/2/5.
//

#ifndef OPLAYER_BASEDECODE_H
#define OPLAYER_BASEDECODE_H


#include "NativeQueue.h"

extern "C" {
#include "libavutil/avutil.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include <libswresample/swresample.h>
}

class BaseDecode {
    /**
       * @param type 解码完成帧回调
       */
    typedef void (*DecodeFrameCallback)(int curTime, int outPCMSize, uint8_t *, int size,
                                        int sampleRate, int channel,
                                        int bit);

    /**
     * @param 初始化成功，需要将音频源数据信息分发下去
     */
    typedef void (*AudioStreamsInfoCallBack)(int sampleRate, int totalTime);

public:
    const char *url = 0;
    int audioStreamIndex = -1;
    AVFormatContext *avFormatContext = NULL;
    AVCodecParameters *avAudioCodecParameters = NULL;

    AVCodecContext *avCodecContext = NULL;
    NativeQueue<AVPacket *> avPacketQueue;//未解码数据
    DecodeFrameCallback decodeFrameCallback;//解码回调

    AudioStreamsInfoCallBack audioStreamsInfoCallBack;//音频流数据信息回调

    pthread_mutex_t init_mutex;
    AVRational time_base;


public:
    BaseDecode();

    ~BaseDecode();

    int init();

    //设置删除帧的回调
    void addDecodeFrameCallback(DecodeFrameCallback decodeFrameCallback) {
        this->decodeFrameCallback = decodeFrameCallback;
    }

    //设置音频数据信息的回调，分发给需要的地方，需要的话，只需要注册回调即可
    void addAudioStreamsInfoCallBack(AudioStreamsInfoCallBack decodecFrameCallback) {
        this->audioStreamsInfoCallBack = decodecFrameCallback;
    }

};


#endif //OPLAYER_BASEDECODE_H
