#include "play_audio.h"

play_audio::play_audio()
: play(play_type::play_type_audio)
{

}

play_audio::~play_audio()
{

}

//创建
bool play_audio::create()
{
    return true;
}

//销毁
void play_audio::destroy()
{

}

//初始化
void play_audio::init()
{
    //以下代码解决同一个进程播放多个音频流
//    m_AudioDevID = SDL_OpenAudioDevice(NULL, 0, &m_AudioParams, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
//    if (m_AudioDevID == 0)
//    {
//    printf("Failed to open audio: %s\n", SDL_GetError());
//    return -1;
//    }
//    SDL_PauseAudioDevice(m_AudioDevID, 0);
}

