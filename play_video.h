#ifndef __DEF_PLAY_VIDEO_H__
#define __DEF_PLAY_VIDEO_H__

#include "play.h"
#include <thread>

//音频视频同步回调
using av_sync_cb = std::function<double()>;



class play_video : public play
{
public:
    explicit play_video();
    virtual ~play_video();

public:
    //创建
    bool create(void* view, av_sync_cb cb);
    //销毁
    void destroy();

private:
    //播放视频
    void paly_video();
    //设置时钟
    void set_clock();

private:
    static void play_thd(void* ctx);


protected:
    //初始化
    virtual void init();

private:
    void*           _view;
    std::thread     _thd;
    av_sync_cb      _sync_cb;

};

#endif // __DEF_PLAY_VIDEO_H__
