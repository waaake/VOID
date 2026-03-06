// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _WINDOWS_AUDIO_STREAM_H
#define _WINDOWS_AUDIO_STREAM_H

/* STD */
#include <cstddef>

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class AudioStream
{
public:
    AudioStream(int samplerate, int channels);
    ~AudioStream();

    bool Start();
    void Stop();
    bool WriteSamples(const unsigned char* buffer, std::size_t size);

private:
    int m_Samplerate, m_Channels;

    IMMDeviceEnumerator* m_Enumerator;
    IMMDevice* m_Device;
    IAudioClient* m_AudioClient;
    IAudioRenderClient* m_AudioRenderClient;
    WAVEFORMATEX* m_Wfx;
    UINT32 m_BufferCount;

private:
    UINT32 AvailableFrames();
};

VOID_NAMESPACE_CLOSE

#endif // _WINDOWS_AUDIO_STREAM_H
