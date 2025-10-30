// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>
#include <cstring>

/* Internal */
#if defined(_WIN32) || defined(__CYGWIN__)
#define NOMINMAX    // miniaudio.h internally includes windows.h which causes conflicts with std::min
#endif // defined(_WIN32) || defined(__CYGWIN__)

#define MINIAUDIO_IMPLEMENTATION
#include "AudioStream.h"
#include "Logging.h"

VOID_NAMESPACE_OPEN

// void AudioStream::DataCallback(ma_device* device, void* output, const void* /* input */, ma_uint32 frameCount)
// {
//     AudioStream* stream = static_cast<AudioStream*>(device->pUserData);
//     size_t bytesPerFrame = ma_get_bytes_per_frame(device->playback.format, device->playback.channels);
//     size_t bytesToRead = frameCount * bytesPerFrame;

//     std::lock_guard<std::mutex> lock(stream->m_Mutex);

//     size_t available = stream->m_Buffer.size() - stream->m_ReadPos;
//     size_t toCopy = std::min(bytesToRead, available);

//     std::memcpy(output, stream->m_Buffer.data() + stream->m_ReadPos, toCopy);
//     stream->m_ReadPos += toCopy;

//     if (stream->m_ReadPos >= stream->m_Buffer.size())
//     {
//         // stream->m_Buffer.clear(); // Do we need to clear everytime ??
//         stream->m_ReadPos = 0;
//     }

//     if (toCopy < bytesToRead)
//     {
//         std::memset((uint8_t*)output + toCopy, 0, bytesToRead - toCopy);
//     }

//     VOID_LOG_INFO("Frame Count: {0}", frameCount);
//     VOID_LOG_INFO("Audio Frame: {0}", stream->m_ReadPos / bytesToRead);
//     VOID_LOG_INFO("Bytes to Read: {0}", bytesToRead);
//     VOID_LOG_INFO("Bytes Per Frame: {0}", bytesPerFrame);
//     VOID_LOG_INFO("---------------------------------------------------");
// }

void AudioStream::DataCallback(ma_device* device, void* output, const void* /* input */, ma_uint32 frameCount)
{
    AudioStream* stream = static_cast<AudioStream*>(device->pUserData);
    size_t bytesPerFrame = ma_get_bytes_per_frame(device->playback.format, device->playback.channels);
    size_t bytesToRead = frameCount * bytesPerFrame;

    unsigned char* out = static_cast<unsigned char*>(output);

    std::lock_guard<std::mutex> lock(stream->m_Mutex);

    while (bytesToRead > 0 && !stream->m_Buffer.empty())
    {
        std::vector<unsigned char>& chunk = stream->m_Buffer.front();
        size_t copySize = std::min(bytesToRead, chunk.size());

        std::copy(chunk.begin(), chunk.begin() + copySize, out);
        out += copySize;

        bytesToRead -= copySize;

        if (copySize < chunk.size())
            chunk.erase(chunk.begin(), chunk.begin() + copySize);
        else
            stream->m_Buffer.pop_front();

        VOID_LOG_INFO("Bytes Read: {0}", copySize);
    }

    if (bytesToRead > 0)
        std::fill(out, out + bytesToRead, 128);
}

AudioStream::AudioStream()
    : m_ReadPos(0)
    , m_Running(false)
{
    std::memset(&m_Device, 0, sizeof(m_Device));
}

AudioStream::~AudioStream()
{
    Stop();
    ma_device_uninit(&m_Device);
}

bool AudioStream::Initialize(uint32_t samplerate, int channels)
{
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_s16;
    config.playback.channels = channels;
    config.sampleRate = samplerate;
    config.dataCallback = DataCallback;
    config.pUserData = this;

    return ma_device_init(nullptr, &config, &m_Device) == MA_SUCCESS;
}

void AudioStream::Start()
{
    // if (!m_Running && !m_Buffer.empty())
    if (!m_Running)
    {
        m_Running = true;
        ma_device_start(&m_Device);

        VOID_LOG_INFO("Started Audio");
    }
}

void AudioStream::Stop()
{
    if (m_Running)
    {
        m_Running = false;
        ma_device_stop(&m_Device);

        VOID_LOG_INFO("Stopped Audio");
    }
}

void AudioStream::Clear()
{
    Stop();

    m_ReadPos = 0;
    m_Buffer.clear();

    VOID_LOG_INFO("Audio Cleared");
}

// void AudioStream::PushPCM(const unsigned char* data, size_t size)
// {
//     std::lock_guard<std::mutex> lock(m_Mutex);
//     m_Buffer.insert(m_Buffer.end(), data, data + size);
// }

// void AudioStream::PushPCM(const std::vector<unsigned char>& data)
// {
//     std::lock_guard<std::mutex> lock(m_Mutex);
//     m_Buffer.insert(m_Buffer.end(), data.begin(), data.end());
// }

// void AudioStream::SetPCM(const std::vector<unsigned char>& stream)
// {
//     std::lock_guard<std::mutex> lock(m_Mutex);
//     m_Buffer = stream;
// }

void AudioStream::PushPCM(const std::vector<unsigned char>& data)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Buffer.push_back(data);
}

VOID_NAMESPACE_CLOSE
