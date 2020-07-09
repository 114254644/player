#ifndef __DEF_PLAY_AUDIO_H__
#define __DEF_PLAY_AUDIO_H__

#include "play.h"

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
};

#endif // __DEF_PLAY_AUDIO_H__
