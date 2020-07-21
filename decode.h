#ifndef __DEF_DECODE_H__
#define __DEF_DECODE_H__


struct decode
{
    bool                hw;
    AVMediaType         type;
    AVPixelFormat       hw_format;
    AVFrame*            frame;
    AVFrame*            in_frame;
    AVFrame*            sw_frame;
    AVFrame*            hw_frame;
    AVBufferRef*        hw_ctx;
    AVStream*           stream;
    AVCodec*            codec;
    AVCodecContext*     decoder;

public:
    decode()
    {
        memset(this, 0, sizeof(decode));
    }
};

bool decode_init(AVMediaType type, AVFormatContext* fmt, decode* dec);
void decode_free(decode* dec);
int  decode_next(decode* dec, AVPacket* pkt, int *got_frame);

#endif // __DEF_DECODE_H__
