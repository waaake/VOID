// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cstring>

/* Internal */
#include "AudioStream.h"
#include "Logging.h"

VOID_NAMESPACE_OPEN

void AudioStream::DataCallback(ma_device* device, void* output, const void* /* input */, ma_uint32 frameCount)
{
    AudioStream* stream = static_cast<AudioStream*>(device->pUserData);
    size_t bytesPerFrame = ma_get_bytes_per_frame(device->playback.format, device->playback.channels);
    size_t bytesToRead = frameCount * bytesPerFrame;

    std::lock_guard<std::mutex> lock(stream->m_Mutex);

    size_t available = stream->m_Buffer.size() - stream->m_ReadPos;
    size_t toCopy = std::min(bytesToRead, available);

    std::memcpy(output, stream->m_Buffer.data() + stream->m_ReadPos, toCopy);
    stream->m_ReadPos += toCopy;

    if (stream->m_ReadPos >= stream->m_Buffer.size())
    {
        // stream->m_Buffer.clear(); // Do we need to clear everytime ??
        stream->m_ReadPos = 0;
    }

    if (toCopy < bytesToRead)
    {
        std::memset((uint8_t*)output + toCopy, 0, bytesToRead - toCopy);
    }
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
    if (!m_Running && !m_Buffer.empty())
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

void AudioStream::PushPCM(const unsigned char* data, size_t size)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Buffer.insert(m_Buffer.end(), data, data + size);
}

void AudioStream::PushPCM(const std::vector<unsigned char>& data)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Buffer.insert(m_Buffer.end(), data.begin(), data.end());
}

void AudioStream::SetPCM(const std::vector<unsigned char>& stream)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Buffer = stream;
}

VOID_NAMESPACE_CLOSE
