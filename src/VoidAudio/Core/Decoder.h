// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _AUDIO_STREAM_DECODER_H
#define _AUDIO_STREAM_DECODER_H

/* STD */
#include <atomic>
#include <future>
#include <string>

/* FFMPEG */
extern "C" 
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

/* Internal */
#include "Definition.h"
#include "VoidAudio/Stream/Linux.h"

VOID_NAMESPACE_OPEN

class VOID_API AudioDecoder
{
public:
    AudioDecoder();
    ~AudioDecoder();

    /**
     * @brief Initialise the buffers and contexts for playback.
     * 
     * @param path Path of the media for audio (could be a video file or an audio file as well)
     */
    void Init(const std::string& path);

    // Start playback if the provided file had valid audio to be played
    void Start();
    // Stop Playback
    void Stop();
    // Reset the PTS and internal time, this makes the audio play from the start of the file
    void Reset();
    // Seeks the audio to the frame in time
    // void SeekTo(v_frame_t frame);
    void SeekTo(double seconds);
    // Returns the current time of the audio read/buffered to the playback server
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
    std::future<void> m_DecodeWorker;

    AudioStream* m_AudioStream;
};

VOID_NAMESPACE_CLOSE

#endif // _AUDIO_STREAM_DECODER_H
