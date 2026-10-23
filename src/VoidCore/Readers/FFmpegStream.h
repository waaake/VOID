// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _FFMPEG_STREAM_H
#define _FFMPEG_STREAM_H

/* STD */
#include <string>
#include <vector>

/* FFmpeg */
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

/* Internal */
#include "Definition.h"
#include "Audio.h"
#include "DecDefs.h"

VOID_NAMESPACE_OPEN

class FFmpegStream
{
public:
    FFmpegStream(AVFormatContext* format, AVStream* stream, int index, const std::string& path);
    ~FFmpegStream();

    bool Setup();

    int64_t CurrentAudioSample() { return m_CurrentAudioSample; }
    int64_t CurrentFrame() { return m_CurrentFrame; }
    int64_t FrameToPts(v_frame_t frame) const;

    AVMediaType Type() const { return m_CodecContext->codec->type; }
    int Index() const { return m_Index; }

    int Width() const { return m_CodecContext->width; }
    int Height() const { return m_CodecContext->height; }
    int Channels() const { return 3; }

    void SendFlushPacket();
    int SendPacket(AVPacket* packet);
    int64_t ReceiveFrame();
    void FlushBuffers();

    bool Decode(DecodedFrame& out);
    bool Decode(AudioBuffer& out);

private:
    std::string m_SourcePath;

    AVFormatContext* m_Format;
    AVStream* m_Stream;
    AVCodecContext* m_CodecContext;
    SwsContext* m_Scaler;
    SwrContext* m_Resampler;
    AVFrame* m_Frame;

    int m_Index;
    int64_t m_CurrentFrame;
    int64_t m_CurrentAudioSample;

private:
    bool SetupResampler();
    std::size_t ResampleAudio(AVFrame* frame, AudioBuffer& buffer, int offset, int samplerate) const;
};

void AVLogError(int errnum);

VOID_NAMESPACE_CLOSE

#endif // _FFMPEG_STREAM_H
