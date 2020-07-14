#ifndef __DEF_PLAY_AUDIO_H__
#define __DEF_PLAY_AUDIO_H__

#include "play.h"
#include "SDL.h"

//采样率sample_rates = 44100
//格式（编码字节数、采样一位所占的字节数） format = s16(格式)=16（bit）
//声道数 channels = 2
//一次采样（一秒中所占的位数）TotalBit = sample_rates * channels * format = 1411200
//一次采样（一秒中所占的字节数）TotalByte = TotalBit/8 = 176400
//1) AAC:
//nb_samples和frame_size = 1024
//一帧数据量：1024*2*s16/8 = 4096个字节。
//ACC帧率 (一秒播放帧数)= TotalByte/4096 = 43.06640625帧
//2) MP3:
//nb_samples和frame_size = 1152
//一帧数据量：1152*2*s16/8 = 4608个字节。
//MP3帧率 (一秒播放帧数)= TotalByte/4608 = 38.28125帧
#define DEF_MAX_LEN     192000

class play_audio : public play
{
public:
    explicit play_audio();
    virtual ~play_audio();

public:
    //创建
    bool create();
    //销毁
    void destroy();

protected:
    //初始化
    virtual void init();

private:
    //音频解码
    int decode_audio();
private:
    //SDL2播放音频回调
    static void play_audio_cb(void* ctx, unsigned char* buf, int len);
    void play_audio_cb(unsigned char* buf, int len);

private:
    int             _buf_index;     //音频数据播放位置
    int             _buf_size;      //音频数据大小
    unsigned char*  _buffer;        //音频数据
    int             _dev_id;        //音频设备ID
    bool            _inited;        //是否已经初始化
    SDL_AudioSpec   _audio_spec;    //音频参数

};

#endif // __DEF_PLAY_AUDIO_H__
