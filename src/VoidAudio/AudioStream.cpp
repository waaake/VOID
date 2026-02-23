// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>
#include <cstring>

#if defined(_WIN32) || defined(__CYGWIN__)
#define NOMINMAX    // miniaudio.h internally includes windows.h which causes conflicts with std::min
#endif // defined(_WIN32) || defined(__CYGWIN__)

/* Miniaudio */
#define MINIAUDIO_IMPLEMENTATION

/* Internal */
#include "AudioStream.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

AudioStream::AudioStream()
    : m_Running(false)
    , m_ReadPos(0ULL)
{
}

AudioStream::~AudioStream()
{
    Stop();
    ma_device_uninit(&m_Device); // sets the memory to 0
}

bool AudioStream::Init(int samplerate, int channels)
{
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_s16;
    config.playback.channels = channels;
    config.sampleRate = samplerate;
    config.dataCallback = &AudioStream::ma_callback;
    config.pUserData = this;

    return ma_device_init(nullptr, &config, &m_Device) == MA_SUCCESS;
}

void AudioStream::Play()
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

void AudioStream::Push(const std::vector<unsigned char>& buffer)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Buffer.insert(m_Buffer.end(), buffer.begin(), buffer.end());
}

void AudioStream::Push(const unsigned char* buffer, std::size_t size)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Buffer.insert(m_Buffer.end(), buffer, buffer + size);
    // VOID_LOG_INFO("Pushing buffer of size: {0}", size);
}

void AudioStream::ma_callback(ma_device* device, void* outbuffer, const void* inbuffer, unsigned int framecount)
{
    AudioStream* stream = static_cast<AudioStream*>(device->pUserData);
    std::size_t bytesToRead = framecount * ma_get_bytes_per_frame(device->playback.format, device->playback.channels);

    std::lock_guard<std::mutex> lock(stream->m_Mutex);

    // std::size_t toCopy = std::min(bytesToRead, stream->m_Buffer.size() - stream->m_ReadPos);
    std::size_t toCopy = std::min(bytesToRead, stream->m_Buffer.size());
    std::size_t remaining = bytesToRead - toCopy;

    VOID_LOG_INFO("Copying {} bytes of the required {} bytes", toCopy, bytesToRead);
    // std::memcpy(outbuffer, stream->m_Buffer.data() + stream->m_ReadPos, toCopy);
    std::memcpy(outbuffer, stream->m_Buffer.data(), toCopy);
    stream->m_Buffer.erase(stream->m_Buffer.begin(), stream->m_Buffer.begin() + toCopy);
    // stream->m_ReadPos += toCopy;

    // if (stream->m_ReadPos >= stream->m_Buffer.size())
    // {
    //     stream->m_Buffer.clear();
    //     stream->m_ReadPos = 0;
    // }

    // if (toCopy < bytesToRead)
    // {
    //     std::memset(outbuffer, 0, bytesToRead - toCopy);
    //     VOID_LOG_INFO("Copying remainging: {} bytes", bytesToRead - toCopy);
    // }
    if (remaining > 0)
    {
        std::memset(outbuffer, 0, remaining);
        VOID_LOG_INFO("Copying remainging: {} bytes", remaining);
    }
}

VOID_NAMESPACE_CLOSE
