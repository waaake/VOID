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

    bool Initialize(uint32_t samplerate = 44100, int channels = 2);
    void PushPCM(const unsigned char* data, size_t size);
    void PushPCM(const std::vector<unsigned char>& data);
    void SetPCM(const std::vector<unsigned char>& stream);

    void Start();
    void Stop();
    inline void Reset() { m_ReadPos = 0; }
    void Clear();

private: /* Members */
    ma_device m_Device;
    std::mutex m_Mutex;
    std::vector<unsigned char> m_Buffer;
    size_t m_ReadPos;
    std::atomic<bool> m_Running;

private: /* Methods */
    static void DataCallback(ma_device* device, void* output, const void* input, ma_uint32 framecount);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_AUDIO_STREAM_H
