// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _FFMPEG_STREAM_H
#define _FFMPEG_STREAM_H

/* STD */
#include <string>

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

    AVRational StreamTimebase() const { return m_Stream->time_base; }
    AVRational Framerate() const { return m_Stream->avg_frame_rate; }

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

    static int64_t OutputSamplerate() { return s_OutputSamplerate; }
    static int OutputChannels() { return s_OutputChannels; }
    static AVRational AudioTimebase() { return s_AudioTimeBase; }
    
    /// @brief Converts the video presentation timestamp into audio sample units
    /// as the both most likely will have different time bases
    /// this is mathematically equal to 
    /// new PTS = old PTS x (old time base / new time base)
    /// @param pts Video presentation timestamp.
    /// @param sourcetimebase Video timebase.
    /// @return Converted audio sample units.
    static int64_t ToAudioSamplePosition(int64_t pts, const AVRational& sourcetimebase);

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

    /// @brief Audio Out params
    static constexpr int64_t s_OutputSamplerate = 48000;
    static constexpr int s_OutputChannels       = 2;
    static constexpr AVRational s_AudioTimeBase = AVRational{1, 48000};

private:
    bool SetupResampler();
    std::size_t ResampleAudio(AVFrame* frame, AudioBuffer& buffer, int offset, int samplerate) const;
};

void AVLogError(int errnum);

VOID_NAMESPACE_CLOSE

#endif // _FFMPEG_STREAM_H
