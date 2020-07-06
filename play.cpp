#include "play.h"

play::play(int type)
: _type((play_type)type)
, _fmt(nullptr)
, _clock(0)
, _volume(0)
, _paused(false)
, _stoped(false)
{

}

play::~play()
{

}

//打开
bool    play::open(AVFormatContext* fmt)
{
    //::TODO
    return true;
}

//关闭
void    playc::close()
{
    //::TODO
}

//暂停
void    play::pause(bool pause)
{
    auto_lock lock(_mtx);
    _paused = pause;
}

//停止
void    play::stop()
{
    auto_lock lock(_mtx);
    _stoped = true;
}

//时钟
double  play::clock()
{
    return _clock;
}

//获取音量
int     play::get_volume()
{
    return _volume;
}

//设置音量
void    play::set_volume(int vol)
{
    _volume = vol;
}

//添加数据据包
void    play::push_packet(AVPacket* pkt)
{
    //::TODO
}

//清理所有的数据包
void    play::flush_packets()
{
    //::TODO
}
