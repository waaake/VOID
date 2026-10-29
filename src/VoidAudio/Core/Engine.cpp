// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Engine.h"

#if defined(_VOID_PLATFORM_WINDOWS)
#include "VoidAudio/Stream/Win.h"
#elif defined(_VOID_PLATFORM_APPLE)
#include "VoidAudio/Stream/Mac.h"
#else
#include "VoidAudio/Stream/Linux.h"
#endif

#include "VoidCore/Logging.h"
#include "Internal/Descriptors.h"

VOID_NAMESPACE_OPEN

AudioEngine::AudioEngine()
{
    #if defined(_VOID_PLATFORM_LINUX)
    m_AudioStream = new AudioStream(Audio::OutSamplerate, Audio::OutChannels, PA_SAMPLE_FLOAT32);
    #else
    m_AudioStream = new AudioStream(Audio::OutSamplerate, Audio::OutChannels);
    #endif // _VOID_PLATFORM_LINUX
}

AudioEngine::~AudioEngine()
{
    Stop();
}

bool AudioEngine::Start()
{
    return m_AudioStream->Start();
}

void AudioEngine::Stop()
{
    m_AudioStream->Stop();
}

void AudioEngine::SetVolume(float volume)
{
    m_AudioStream->SetVolume(volume);
}

void AudioEngine::Play(const SharedFrameAudio& buffer)
{
    // VOID_LOG_INFO("Writing {} bytes with {} samplecount", buffer->Bytes(), buffer->samplecount);
    #if defined(_VOID_PLATFORM_LINUX)
    m_AudioStream->WriteSamples(buffer->Data(), buffer->Bytes(), buffer->samplecount);
    #else
    m_AudioStream->WriteSamples(buffer->Data(), buffer->Size(), buffer->samplecount);
    #endif
}

VOID_NAMESPACE_CLOSE
