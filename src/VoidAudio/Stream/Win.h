// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _WINDOWS_AUDIO_STREAM_H
#define _WINDOWS_AUDIO_STREAM_H

/* Windows */
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
    bool WriteSamples(const unsigned char* buffer, std::size_t size, int samples);
    bool WriteSamples(const void* buffer, std::size_t size, int samples);
    void SeekTo(double time) {}

    void SetVolume(float volume);
    double CurrentTime() const { return 0.0; }
    double Latency() const;

private:
    IMMDeviceEnumerator* m_Enumerator;
    IMMDevice* m_Device;
    IAudioClient* m_AudioClient;
    IAudioRenderClient* m_AudioRenderClient;
    WAVEFORMATEX* m_Wfx;
    UINT32 m_BufferCount;

    int m_Samplerate, m_Channels;
    float m_Volume;

private:
    UINT32 AvailableFrames();
};

VOID_NAMESPACE_CLOSE

#endif // _WINDOWS_AUDIO_STREAM_H
