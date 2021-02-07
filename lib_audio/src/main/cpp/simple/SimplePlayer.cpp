//
// Created by Administrator on 2021/2/5.
//

#include "SimplePlayer.h"


SimplePlayer::SimplePlayer() {

}

SimplePlayer::~SimplePlayer() {

}

jobject initCreateAudioTrack(JNIEnv *env) {
    /*AudioTrack(int streamType, int sampleRateInHz, int channelConfig, int audioFormat,
            int bufferSizeInBytes, int mode)*/
    jclass jAudioTrackClass = env->FindClass("android/media/AudioTrack");
    jmethodID jAudioTackCMid = env->GetMethodID(jAudioTrackClass, "<init>", "(IIIIII)V");

    int streamType = 3;
    int sampleRateInHz = AUDIO_SAMPLE_RATE;
    int channelConfig = (0x4|0x8);
    int audioFormat = 2;
    int mode = 1;

    // int getMinBufferSize(int sampleRateInHz, int channelConfig, int audioFormat)
    jmethodID getMinBufferSizeMid = env->GetStaticMethodID(jAudioTrackClass, "getMinBufferSize",
                                                           "(III)I");
    int bufferSizeInBytes = env->CallStaticIntMethod(jAudioTrackClass, getMinBufferSizeMid,
                                                     sampleRateInHz, channelConfig, audioFormat);
    LOGE("bufferSizeInBytes = %d", bufferSizeInBytes);

    jobject jAudioTrackObj = env->NewObject(jAudioTrackClass, jAudioTackCMid, streamType,
                                            sampleRateInHz, channelConfig, audioFormat,
                                            bufferSizeInBytes, mode);

    // play
    jmethodID playMid = env->GetMethodID(jAudioTrackClass, "play", "()V");
    env->CallVoidMethod(jAudioTrackObj, playMid);

    return jAudioTrackObj;
}


void SimplePlayer::player(JNIEnv *env, const char *url) {


    int ret = 0;
    avformat_network_init();

    AVFormatContext *pFormatContext = nullptr;

    //打开文件
    ret = avformat_open_input(&pFormatContext, url, nullptr, nullptr);
    if (ret != 0) {
        LOGE("avformat_open_input:%s", av_err2str(ret));
        return;
    }

    //查找流信息
    ret = avformat_find_stream_info(pFormatContext, nullptr);
    if (ret < 0) {
        LOGE("avformat_find_stream_info");
        return;
    }


    //查找audio
    int audio_stream_index = 0;
    audio_stream_index = av_find_best_stream(pFormatContext, AVMediaType::AVMEDIA_TYPE_AUDIO, -1,
                                             -1, nullptr, 0);
    if (audio_stream_index < 0) {
        LOGE("av_find_best_stream error");
    }


    // 查找解码器 AVCodec
    AVCodecContext *pCodecContext = nullptr;
    AVCodecParameters *pCodecParameters = pFormatContext->streams[audio_stream_index]->codecpar;
    AVCodec *pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (!pCodec) {
        LOGE("codec find audio decoder error");
        return;
    }

    // 打开解码器
    pCodecContext = avcodec_alloc_context3(pCodec);
    if (!pCodecContext) {
        LOGE("codec alloc context error");
        return;
    }

    ret = avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    if (ret < 0) {
        LOGE("codec parameters to context error: %s", av_err2str(ret));
        return;
    }

    ret = avcodec_open2(pCodecContext, pCodec, nullptr);
    if (ret != 0) {
        LOGE("codec audio open error: %s", av_err2str(ret));
        return;
    }


    //创建播放器
    jclass jAudioTrackClass = env->FindClass("android/media/AudioTrack");
    jmethodID jWriteMid = env->GetMethodID(jAudioTrackClass, "write", "([BII)I");
    jobject jAudioTrackObj = initCreateAudioTrack(env);




    //重采样

    int64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    enum AVSampleFormat out_sample_fmt = AVSampleFormat::AV_SAMPLE_FMT_S16;
    int out_sample_rate = AUDIO_SAMPLE_RATE;

    int64_t in_ch_layout = pCodecContext->channel_layout;
    enum AVSampleFormat in_sample_fmt = pCodecContext->sample_fmt;
    int in_sample_rate = pCodecContext->sample_rate;

    LOGE("swrContext in_ch_layout: %lld in_sample_rate: %d",in_ch_layout,in_sample_rate);

    SwrContext *swrContext = swr_alloc();
    swrContext = swr_alloc_set_opts(swrContext, out_ch_layout, out_sample_fmt,
                                    out_sample_rate, in_ch_layout, in_sample_fmt,
                                    in_sample_rate, 0, nullptr);
    if (swrContext == nullptr) {
        LOGE("swrContext ");
        return;
    }
    ret = swr_init(swrContext);
    if (ret < 0) {
        LOGE("swrContext %s", av_err2str(ret));
        return;
    }


    //获取输出size
    int outChannels = av_get_channel_layout_nb_channels(out_ch_layout);
    int dataSize = av_samples_get_buffer_size(nullptr, outChannels, pCodecParameters->frame_size,
                                              out_sample_fmt, 0);
    uint8_t *resampleOutBuffer = (uint8_t *) malloc(dataSize);

    // ---------- 重采样 end ----------






    LOGE("av_read_frame start %d", env->GetVersion());
    jbyteArray jPcmByteArray = env->NewByteArray(dataSize);
    // native 创建 c 数组
    jbyte *jPcmData = env->GetByteArrayElements(jPcmByteArray, nullptr);


    AVPacket *pPacket = nullptr;
    AVFrame *pFrame = nullptr;

    //循环解码
    pPacket = av_packet_alloc();
    pFrame = av_frame_alloc();
    int index = 0;

    LOGE("av_read_frame start");
    while (av_read_frame(pFormatContext, pPacket) >= 0) {

        if (pPacket->stream_index == audio_stream_index) {
            // Packet 包，解码成 pcm 数据

            ret = avcodec_send_packet(pCodecContext, pPacket);
            if (ret == 0) {
                ret = avcodec_receive_frame(pCodecContext, pFrame);
                if (ret == 0) {
                    index++;
                    LOGE("解码第 %d 帧", index);
                    // 调用重采样的方法
                    swr_convert(swrContext, &resampleOutBuffer,  pFrame->nb_samples,
                                (const uint8_t **) pFrame->data, pFrame->nb_samples);

                    memcpy(jPcmData, resampleOutBuffer, dataSize);
                    // 0 把 c 的数组的数据同步到 jbyteArray , 然后释放native数组
                    env->ReleaseByteArrayElements(jPcmByteArray, jPcmData, JNI_COMMIT);
                    env->CallIntMethod(jAudioTrackObj, jWriteMid, jPcmByteArray, 0, dataSize);
                } else {
                    LOGE("avcodec_receive_frame");
                }
            } else {
                LOGE("avcodec_send_packet");
            }
        }
        // 解引用
        av_packet_unref(pPacket);
        av_frame_unref(pFrame);
    }
    LOGE("av_read_frame end");

    // 1. 解引用数据 data ， 2. 销毁 pPacket 结构体内存  3. pPacket = NULL
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);


    if (pCodecContext) {
        avcodec_close(pCodecContext);
        avcodec_free_context(&pCodecContext);
        pCodecContext = nullptr;
    }

    if (pFormatContext) {
        avformat_close_input(&pFormatContext);
        avformat_free_context(pFormatContext);
        pFormatContext = nullptr;
    }
    avformat_network_deinit();

}
