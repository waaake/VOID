// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cstring>

/* Internal */
#include "AudioStream.h"

VOID_NAMESPACE_OPEN

AudioStream::AudioStream()
{
    std::memset(&m_Device, 0, sizeof(m_Device));
}

AudioStream::~AudioStream()
{
    Stop();
    ma_device_uninit(&m_Device);
}

bool AudioStream::Initialize(int samplerate, int channels, ma_format format)
{
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = format;
    config.playback.channels = channels;
    config.sampleRate = samplerate;
    config.dataCallback = DataCallback;
    config.pUserData = this;

    // if (ma_device_init(nullptr, &config, &m_Device) != MA_SUCCESS)
    //     return false;
    // return true;
    return ma_device_init(nullptr, &config, &m_Device); 
}

void AudioStream::Start()
{
    if (!m_Running)
    {
        m_Running = true;
        ma_device_start(&m_Device);
    }
}

void AudioStream::Stop()
{
    if (m_Running)
    {
        m_Running = false;
        ma_device_stop(&m_Device);
    }
}

void AudioStream::PushPCM(const unsigned char* data, size_t size)
{
    std::lock_guard<std::mutex> guard(m_Mutex);
    m_PcmQueue.push(std::vector<unsigned char>(data, data + size));
}

void AudioStream::DataCallback(ma_device* device, void* output, const void* /* input */, unsigned int framecount)
{
    AudioStream* stream = reinterpret_cast<AudioStream*>(device->pUserData);
    std::lock_guard<std::mutex> guard(stream->m_Mutex);

    unsigned char* out = static_cast<unsigned char*>(output);
    size_t bytesNeeded = framecount + ma_get_bytes_per_frame(device->playback.format, device->playback.channels);
    size_t bytesWritten = 0;

    while (!stream->m_PcmQueue.empty() && bytesWritten < bytesNeeded)
    {
        std::vector<unsigned char>& chunk = stream->m_PcmQueue.front();
        size_t copySize = std::min(chunk.size(), bytesNeeded - bytesWritten);

        std::memcpy(out + bytesWritten, chunk.data(), copySize);
        bytesWritten += copySize;

        if (copySize < chunk.size())
            chunk.erase(chunk.begin(), chunk.begin() + copySize);
        else
            stream->m_PcmQueue.pop();
    }

    /* Zero the bytes that are missing */
    if (bytesWritten < bytesNeeded)
        std::memset(out + bytesWritten, 0, bytesNeeded - bytesWritten);
}

VOID_NAMESPACE_CLOSE
