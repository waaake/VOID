// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _AUDIO_STREAM_H
#define _AUDIO_STREAM_H

/* STD */
#include <mutex>
#include <vector>

#include "extern/miniaudio.h"

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VOID_API AudioStream
{
public:
    AudioStream();
    ~AudioStream();

    bool Init(int samplerate, int channels);
    void Push(const std::vector<unsigned char>& buffer);
    void Push(const unsigned char* buffer, std::size_t size);
    inline void Reserve(std::size_t size) { m_Buffer.reserve(size); }

    void Play();
    void Stop();

private: /* Members */
    ma_device m_Device;
    std::mutex m_Mutex;
    std::vector<unsigned char> m_Buffer;
    bool m_Running;
    std::size_t m_ReadPos;

private: /* Methods */
    static void ma_callback(ma_device* device, void* outbuffer, const void* inbuffer, unsigned int framecount);
};

VOID_NAMESPACE_CLOSE

#endif // _AUDIO_STREAM_H
