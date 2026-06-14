// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _FFMPEG_PIX_WRITER_H
#define _FFMPEG_PIX_WRITER_H

/* FFmpeg */
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

/* Internal */
#include "Definition.h"
#include "PixWriter.h"

VOID_NAMESPACE_OPEN

class VOID_API FFmpegWriter : public PixWriter
{
public:
    FFmpegWriter(const EncodeSpec& spec);
    ~FFmpegWriter();

    bool Setup(const std::string& path) override;
    bool AddBuffer(const void* buffer, std::size_t size, const InputSpec& spec) override;
    bool Write() override;
    void Cleanup() override;

private: /* Members */
    AVFormatContext* m_FormatCtx;
    AVCodecContext* m_CodecCtx;
    SwsContext* m_SwsCtx;
    AVStream* m_Stream;
    int64_t m_Pts;
    AVFrame* m_SourceFrame;
    AVFrame* m_DestinationFrame;

    std::string m_Path;

private: /* Methods */
    AVCodecID Codec();
    AVPixelFormat PixelFormat();
    void ContextSetup();
    void InitFrameContext(const InputSpec& spec);

    void CopyBuffer(const uint8_t* src, uint8_t* dest, std::size_t size);
    void CopyBuffer(const uint16_t* src, uint8_t* dest, std::size_t size);
    void CopyBuffer(const float* src, uint8_t* dest, std::size_t size);
};

VOID_NAMESPACE_CLOSE

#endif // _FFMPEG_PIX_WRITER_H
