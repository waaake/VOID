// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cstring>
// #include <algorithm>
#include <thread>

/* Internal */
#include "Win.h"
#include "VoidCore/Logging.h"

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
    return m_AudioClient->Start();
}

void AudioStream::Stop()
{
    m_AudioClient->Stop();
}

bool AudioStream::WriteSamples(const unsigned char* buffer, std::size_t size)
{    
    REFERENCE_TIME hnsDefaultDevicePeriod;
    REFERENCE_TIME hnsMinimumDevicePeriod;
    HRESULT hr = m_AudioClient->GetDevicePeriod(&hnsDefaultDevicePeriod, &hnsMinimumDevicePeriod);

    UINT32 framesAvailable = AvailableFrames();
    VOID_LOG_INFO("Available: {0}", framesAvailable);

    if (framesAvailable)
    {
        BYTE* data;
        hr = m_AudioRenderClient->GetBuffer(framesAvailable, &data);
    
        if (FAILED(hr))
        {
            VOID_LOG_INFO("GetBuffer failed.");
            return false;
        }
    
        const int16_t* in = reinterpret_cast<const int16_t*>(buffer);
        std::size_t inputSamples = size / sizeof(int16_t);
    
        float* out = reinterpret_cast<float*>(data);
        std::size_t outSamples = framesAvailable * m_Wfx->nBlockAlign;
    
        std::size_t toCopy = (inputSamples < outSamples) ? inputSamples : outSamples;
    
        for (std::size_t i = 0; i < toCopy; ++i)
            out[i] = in[i] / 32767.0f; // Normalize to [-1.0 , 1.0]
        
        // Release the buffer back to WAS for playing
        m_AudioRenderClient->ReleaseBuffer(framesAvailable, 0);
    }
    else
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(hnsDefaultDevicePeriod / 10000));
        // VOID_LOG_INFO("--{0}", m_Wfx->nSamplesPerSec);
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
