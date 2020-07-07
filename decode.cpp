#include "decode.h"

static int open_codec(decode *d, bool hw)
{
    AVCodecContext *c;
        int ret;

    #if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 40, 101)
        c = avcodec_alloc_context3(d->codec);
        if (!c)
            return -1;

        ret = avcodec_parameters_to_context(c, d->stream->codecpar);
        if (ret < 0)
            goto fail;
    #else
        c = d->stream->codec;
    #endif
        d->hw = false;
    #ifdef USE_NEW_HARDWARE_CODEC_METHOD
//        if (hw)
//            init_hw_decoder(d, c);
    #endif

        if (c->thread_count == 1 && c->codec_id != AV_CODEC_ID_PNG &&
            c->codec_id != AV_CODEC_ID_TIFF &&
            c->codec_id != AV_CODEC_ID_JPEG2000 &&
            c->codec_id != AV_CODEC_ID_MPEG4 && c->codec_id != AV_CODEC_ID_WEBP)
            c->thread_count = 0;

        ret = avcodec_open2(c, d->codec, NULL);
        if (ret < 0)
            goto fail;

        d->decoder = c;
        return ret;

    fail:
        avcodec_close(c);
    #if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 40, 101)
        av_free(d->decoder);
    #endif
        return ret;
}

bool decode_init(AVMediaType type, AVFormatContext* fmt, decode* dec)
{
    if(!dec || !fmt)
       return false;

    memset(dec, 0, sizeof(*dec));
    dec->type = type;
    ret = av_find_best_stream(fmt, type, -1, -1, nullptr, 0);
    if (ret < 0)
        return false;

    AVCodecID id;
    dec->stream = d->stream = fmt->streams[ret];
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 40, 101)
    id = stream->codecpar->codec_id;
#else
    id = stream->codec->codec_id;
#endif
    if (!dec->codec)
        dec->codec = avcodec_find_decoder(id);

    if (!dec->codec)
        return false;

    ret = open_codec(dec, hw);
    if (ret < 0)
        return false;

    dec->frame = av_frame_alloc();
    if (dec->codec->capabilities & CODEC_CAP_TRUNC)
        dec->decoder->flags |= CODEC_FLAG_TRUNC;
    return true;
}

void decode_free(decode* dec)
{
    if(!dec)
       return;

    if(dec->frame)
    {
        av_frame_unref(dec->frame);
        av_free(dec->frame);
    }

    if (dec->decoder)
    {
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 40, 101)
        avcodec_free_context(&dec->decoder);
#else
        avcodec_close(dec->decoder);
#endif
    }

    memset(ddec, 0, sizeof(*dec));
}

int decode_next(decode* dec, AVPacket* pkt, int *got_frame)
{
    int ret = -1;
    *got_frame = 0;

    if(!dec || !pkt)
        return ret;

#ifdef USE_NEW_FFMPEG_DECODE_API
    ret = avcodec_receive_frame(dec->decoder, dec->frame);
    if (ret != 0 && ret != AVERROR(EAGAIN)) {
        if (ret == AVERROR_EOF)
            ret = 0;
        return ret;
    }

    if (ret != 0) {
        ret = avcodec_send_packet(dec->decoder, pkt);
        if (ret != 0 && ret != AVERROR(EAGAIN)) {
            if (ret == AVERROR_EOF)
                ret = 0;
            return ret;
        }

        ret = avcodec_receive_frame(d->decoder, d->frame);
        if (ret != 0 && ret != AVERROR(EAGAIN)) {
            if (ret == AVERROR_EOF)
                ret = 0;
            return ret;
        }

        *got_frame = (ret == 0);
        ret = pkt.size;
    } else {
        ret = 0;
        *got_frame = 1;
    }

#else
    if (AVMEDIA_TYPE_AUDIO  == dec->type) {
        ret = avcodec_decode_audio4(dec->decoder, dec->frame, got_frame, pkt);
    } else if(AVMEDIA_TYPE_VIDEO == dec->type) {
        ret = avcodec_decode_video2(dec->decoder, dec->frame, got_frame, pkt);
    }
#endif

    return ret;
}
