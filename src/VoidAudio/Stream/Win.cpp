// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cstring>
// #include <algorithm>
#include <thread>

/* Internal */
#include "Win.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"

VOID_NAMESPACE_OPEN

AudioStream::AudioStream(int samplerate, int channels)
    : m_Samplerate(samplerate)
    , m_Channels(channels)
    , m_Enumerator(nullptr)
    , m_Device(nullptr)
    , m_AudioClient(nullptr)
    , m_AudioRenderClient(nullptr)
    , m_Wfx(nullptr)
    , m_BufferCount(0)
{
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&m_Enumerator);
    m_Enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_Device);

    m_Device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&m_AudioClient);
    m_AudioClient->GetMixFormat(&m_Wfx);
    m_AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, m_Wfx, NULL);
    m_AudioClient->GetService(__uuidof(IAudioRenderClient), (void**)&m_AudioRenderClient);
    m_AudioClient->GetBufferSize(&m_BufferCount);
}


AudioStream::~AudioStream()
{
    if (m_Wfx) CoTaskMemFree(m_Wfx);
    if (m_AudioRenderClient) m_AudioRenderClient->Release();
    if (m_AudioClient) m_AudioClient->Release();
    if (m_Device) m_Device->Release();
    if (m_Enumerator) m_Enumerator->Release();
    CoUninitialize();
}

bool AudioStream::Start()
{
    HRESULT hr = m_AudioClient->Start();
    return FAILED(hr) ? false : true;
}

void AudioStream::Stop()
{
    m_AudioClient->Stop();
}

bool AudioStream::WriteSamples(const unsigned char* buffer, std::size_t size)
{
    Tools::VoidProfiler<std::chrono::milliseconds> p("AudioStream::WriteSamples");

    const int16_t* in = reinterpret_cast<const int16_t*>(buffer);
    std::size_t samples = size / sizeof(int16_t);
    UINT32 inframes = samples / m_Wfx->nChannels;

    UINT32 framesAvailable;
    while (framesAvailable < inframes)
    {
        framesAvailable = AvailableFrames();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    if (framesAvailable > 0)
    {
        BYTE* data;
        HRESULT hr = m_AudioRenderClient->GetBuffer(framesAvailable, &data);

        if (FAILED(hr))
        {
            VOID_LOG_INFO("GetBuffer failed");
            return false;
        }

        float* out = reinterpret_cast<float*>(data);

        UINT32 frames = (framesAvailable < inframes) ? framesAvailable : inframes;

        for (UINT32 i = 0; i < frames * m_Wfx->nChannels; ++i)
            out[i] = in[i] / 32768.0f;

        m_AudioRenderClient->ReleaseBuffer(frames, 0);
    }

    return true;
}

UINT32 AudioStream::AvailableFrames()
{
    UINT32 framesPadding;
    m_AudioClient->GetCurrentPadding(&framesPadding);
    return m_BufferCount - framesPadding;
}

VOID_NAMESPACE_CLOSE
