// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _FFMPEG_PIX_WRITER_H
#define _FFMPEG_PIX_WRITER_H

/* FFmpeg */
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
}

/* Internal */
#include "Definition.h"
#include "PixWriter.h"

VOID_NAMESPACE_OPEN

class VOID_API FFmpegWriter : public PixWriter
{
public:
    FFmpegWriter(const EncodeSpec& spec);

    bool Setup(const std::string& path) override;
    bool AddBuffer(const void* buffer, std::size_t size, const InputSpec& type) override;
    bool Write() override;
    void Cleanup() override;

private: /* Members */
    AVFormatContext* m_FormatCtx;
    AVCodecContext* m_CodecCtx;
    SwsContext* m_SwsCtx;
    AVStream* m_Stream;
    int64_t m_Pts;

    std::string m_Path;

private: /* Methods */
    AVCodecID Codec();
    AVPixelFormat PixelFormat();
    void ContextSetup();
};

VOID_NAMESPACE_CLOSE

#endif // _FFMPEG_PIX_WRITER_H
