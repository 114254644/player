#ifndef __DEF_PLAY_H__
#define __DEF_PLAY_H__

#include <mutex>
#include "decode.h"
struct AVPacket;
struct AVFormatContext;
using auto_lock = std::lock_guard<std::mutex>;

class play
{
public:
    enum class play_type
    {
        play_type_unknown = -1,     //未知
        play_type_audio,            //音频
        play_type_video,            //视频
        play_type_subtitle          //字幕
    };
public:
    explicit play(int type);
    virtual ~play();

public:
    //打开
    bool    open(AVFormatContext* fmt);
    //关闭
    void    close();
    //暂停
    void    pause(bool pause);
    //停止
    void    stop();
    //时钟
    double  clock();
    //获取音量
    int     get_volume();
    //设置音量
    void    set_volume(int vol);
    //添加数据据包
    void    push_packet(AVPacket* pkt);
    //清理所有的数据包
    void    flush_packets();

protected:
    //初始化
    virtual void init();
protected:
    int                 _volume;
    double              _clock;
    bool                _paused;
    bool                _stoped;
    decode              _decode;
    play_type           _type;

private:
    std::mutex          _mtx;
    AVFormatContext*    _fmt;
}

#endif // __DEF_PLAY_H__
