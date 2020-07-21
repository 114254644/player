#include "decode.h"

#if LIBAVCODEC_VERSION_INT > AV_VERSION_INT(58, 4, 100)
#define USE_NEW_HARDWARE_CODEC_METHOD
#endif

#ifdef USE_NEW_HARDWARE_CODEC_METHOD
enum AVHWDeviceType hw_priority[] = {
    AV_HWDEVICE_TYPE_D3D11VA, AV_HWDEVICE_TYPE_DXVA2,
    AV_HWDEVICE_TYPE_VAAPI,   AV_HWDEVICE_TYPE_VDPAU,
    AV_HWDEVICE_TYPE_QSV,     AV_HWDEVICE_TYPE_NONE,
};

static bool has_hw_type(AVCodec *c, enum AVHWDeviceType type,
            enum AVPixelFormat *hw_format)
{
    for (int i = 0;; i++) {
        const AVCodecHWConfig *config = avcodec_get_hw_config(c, i);
        if (!config) {
            break;
        }

        if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
            config->device_type == type) {
            *hw_format = config->pix_fmt;
            return true;
        }
    }

    return false;
}

static void init_hw_decoder(decode *d, AVCodecContext *c)
{
    enum AVHWDeviceType *priority = hw_priority;
    AVBufferRef *hw_ctx = NULL;

    while (*priority != AV_HWDEVICE_TYPE_NONE) {
        if (has_hw_type(d->codec, *priority, &d->hw_format)) {
            int ret = av_hwdevice_ctx_create(&hw_ctx, *priority,
                             NULL, NULL, 0);
            if (ret == 0)
                break;
        }

        priority++;
    }

    if (hw_ctx) {
        c->hw_device_ctx = av_buffer_ref(hw_ctx);
        c->opaque = d;
        d->hw_ctx = hw_ctx;
        d->hw = true;
    }
}
#endif

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
        if (hw)
            init_hw_decoder(d, c);
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
    if (id == AV_CODEC_ID_VP8 || id == AV_CODEC_ID_VP9)
    {
        AVDictionaryEntry *tag = NULL;
        tag = av_dict_get(stream->metadata, "alpha_mode", tag, AV_DICT_IGNORE_SUFFIX);
        if (tag && strcmp(tag->value, "1") == 0)
        {
            char *codec = (id == AV_CODEC_ID_VP8) ? "libvpx" : "libvpx-vp9";
            dec->codec = avcodec_find_decoder_by_name(codec);
        }
    }
    if (!dec->codec)
        dec->codec = avcodec_find_decoder(id);

    if (!dec->codec)
        return false;

    ret = open_codec(dec, hw);
    if (ret < 0)
        return false;

   dec->sw_frame = av_frame_alloc();
   if (!d->sw_frame)
        return false;
   if (dec->hw) {
          dec->hw_frame = av_frame_alloc();
           if (!dec->hw_frame) {
               return false;
           }

           dec->in_frame = dec->hw_frame;
       } else {
           dec->in_frame = dec->sw_frame;
       }

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

#ifdef USE_NEW_HARDWARE_CODEC_METHOD
    if (*got_frame && dec->hw) {
        if (dec->hw_frame->format != dec->hw_format) {
            dec->frame = dec->hw_frame;
            return ret;
        }

        int err = av_hwframe_transfer_data(dec->sw_frame, dec->hw_frame, 0);
        if (err != 0) {
            ret = 0;
            *got_frame = false;
        }
    }
#endif
    dec->frame = dec->sw_frame;
    return ret;
}
