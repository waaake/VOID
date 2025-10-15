// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_AUDIO_STREAM_H
#define _VOID_AUDIO_STREAM_H

/* STD */
#include <atomic>
#include <mutex>
#include <queue>

/* MiniAudio */
#include "miniaudio/miniaudio.h"

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class AudioStream
{
public:
    AudioStream();
    ~AudioStream();

    bool Initialize(int samplerate, int channels, ma_format format);
    void PushPCM(const unsigned char* data, size_t size);

    void Start();
    void Stop();

    // inline unsigned long long PlaybackTime() const { return ma_device_get_total_run_time_in_frames__null(&m_Device); }

private: /* Members */
    ma_device m_Device;
    std::mutex m_Mutex;
    std::queue<std::vector<unsigned char>> m_PcmQueue;
    std::atomic<bool> m_Running;

private: /* Methods */
    static void DataCallback(ma_device* device, void* output, const void* input, ma_uint32 framecount);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_AUDIO_STREAM_H
