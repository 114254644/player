#include "play_audio.h"

play_audio::play_audio()
: play(play_type::play_type_audio)
, _buf_index(0)
, _buf_size(0)
, _buffer(nullptr)
, _inited(false)
, _volume(SDL_MIX_MAXVOLUME)
{
    _buffer = nullptr;
    memset(&_audio_spec, 0, sizeof(_audio_spec));
}

play_audio::~play_audio()
{

}

//创建
bool play_audio::create()
{
    //初始化音频数据
    _audio_spec.freq        = 48000;
    _audio_spec.channels    = 2;
    _audio_spec.format      = AUDIO_F32SYS;
    _audio_spec.silence     = 0;
    //_audio_spec.samples     = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wantedSpec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
    _audio_spec.samples     = 1024;
    _audio_spec.callback    = &play_audio::play_audio_cb;
    _audio_spec.userdata    = this;

    //同一进程播放多路音频
    _dev_id = SDL_OpenAudioDevice(nullptr, 0, &_audio_spec,
                                  nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if(_dev_id < 0)
        return false;

    if(0 == _dev_id)
        SDL_PauseAudio(0);
    else
        SDL_PauseAudioDevice(_dev_id, 0);

    return true;
}

//销毁
void play_audio::destroy()
{
    SDL_LockAudio();
    SDL_CloseAudio();
    SDL_UnlockAudio();
}

//初始化
void play_audio::init()
{
    //重置采样
    if(_decode.decoder->sample_rate != _audio_spec.freq
                || _decode.decoder->channels != _audio_spec.channels
                || _decode.decoder->sample_fmt != AV_SAMPLE_FMT_FLT)
    {
        //::TODO
    }

    _inited = true;
}

//SDL2播放音频回调
void play_audio::play_audio_cb(void* ctx, unsigned char* buf, int len)
{
    play_audio* audio = (play_audio*)ctx;
    if(audio != nullptr)
        audio->play_audio_cb(buf, len);
}

void play_audio::play_audio_cb(unsigned char* buf, int len)
{
    memset(stream, 0, len);
    while(len > 0)
    {
        if(_stoped || !_inited)
            return;
        if(_paused)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        //缓冲区中无数据
        if(_buf_index >= _buf_size)
        {
            //解码
            int dec_size = decode_audio();
            //无数据播放静音
            if(dec_size < 0)
            {
                _buf_size = 1024;
                _buffer = nullptr;
            }
            else
            {
                _buf_size = dec_size;
            }
            _buf_index = 0;
        }

        //计算未播放的数据长度
        int left = _buf_size - _buf_index;
        left = left > len ? len : left;
        if(_buffer != nullptr)
            SDL_MixAudio(stream, _buffer+ _buf_index, left, _volume);

        len -= left;
        stream += left;
        _buf_index += left;
    }
}

//音频解码
int play_audio::decode_audio()
{

}
