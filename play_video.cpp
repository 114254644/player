#include "play_video.h"

play_video::play_video()
: play(play_type::play_type_video)
{

}

play_video::~play_video()
{

}

//创建
bool play_video::create(void* view, av_sync_cb cb)
{
    _view = view;
    _sync_cb = cb;
    return true;
}

//销毁
void play_video::destroy()
{
    if(_thd.joinable())
    {
        _thd.join();
    }
}

//初始化
void play_video::init()
{
    if(_thd.joinable())
        return;

    _thd = std::thread(std::bind(play_video::play_thd, this));
}

void play_video::play_thd(void* ctx)
{
    play_video* video = (play_video*)ctx;
    if(video)
        video->paly_video();
}

void play_video::paly_video()
{
    while(true)
    {
        //停止
        if(_stoped)
            break;
        //暂停
        if(_paused)
        {
           //休眠10毫秒
           std::this_thread::sleep_for(std::chrono::milliseconds(10));
           continue;
        }
        //::TODO解码
        //set_clock
        //同步音视频
        if(_sync_cb)
        {
             while(true)
             {
                 //停止
                 if(_stoped)
                    break;

                 double delay = _sync_cb();
                 if(delay <= 0)
                    break;

                 delay = delay * 1000;
                 delay = delay > 5 ? 5 : delay;
                 std::this_thread::sleep_for(std::chrono::milliseconds(delay));
             }
        }

    }

}

//设置时钟
void play_video::set_clock(double repeat_pict, double pts)
{
   double delay;
   if(pts != 0)
   {
       //Get pts,then set video clock to it
      _clock = pts;
   }
   else
   {
      // Don't get pts,set it to video clock
      pts = _clock;
   }

   delay = av_q2d(_decode.decoder->time_base);
   delay += repeat_pict * (delay * 0.5);
   _clock += delay;
}
