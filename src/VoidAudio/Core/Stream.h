// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _AUDIO_STREAM_DECODER_H
#define _AUDIO_STREAM_DECODER_H

/* STD */
#include <atomic>
#include <string>

/* FFMPEG */
extern "C" 
{
#include <libavformat/avformat.h>
// #include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
// #include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VOID_API AudioDecoder
{
public:
    AudioDecoder();
    ~AudioDecoder();

    void Init(const std::string& path);
    void Start();
    void Stop();
    void Reset();
    double CurrentTime() const { return m_Time.load(); }

private: /* Methods */
    void Cleanup();
    void DecodeSamples();

private: /* Members */
    std::atomic<double> m_Time;
    std::atomic<bool> m_Running;
    std::atomic<bool> m_Finished;

    int m_StreamID;
    AVFormatContext* m_FormatContext;
    AVCodecContext* m_CodecContext;
    AVStream* m_Stream;
    AVFrame* m_Frame;
    AVPacket* m_Packet;
    SwrContext* m_SwrContext;

    bool m_Valid;
};

VOID_NAMESPACE_CLOSE

#endif // _AUDIO_STREAM_DECODER_H
