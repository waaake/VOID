// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Player.h"

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

AudioPlayer::AudioPlayer()
{
    #if defined(_VOID_PLATFORM_LINUX)
    m_AudioStream = new AudioStream(Audio::OutSamplerate, Audio::OutChannels, PA_SAMPLE_FLOAT32);
    #else
    m_AudioStream = new AudioStream(Audio::OutSamplerate, Audio::OutChannels);
    #endif // _VOID_PLATFORM_LINUX
}

AudioPlayer::~AudioPlayer()
{
    Stop();
}

bool AudioPlayer::Start()
{
    return m_AudioStream->Start();
}

void AudioPlayer::Stop()
{
    m_AudioStream->Stop();
}

void AudioPlayer::Play(const SharedFrameAudio& buffer)
{
    // VOID_LOG_INFO("Writing {} bytes with {} samplecount", buffer->Bytes(), buffer->samplecount);
    #if defined(_VOID_PLATFORM_LINUX)
    m_AudioStream->WriteSamples(buffer->Data(), buffer->Bytes(), buffer->samplecount);
    #else
    m_AudioStream->WriteSamples(buffer->Data(), buffer->Size(), buffer->samplecount);
    #endif
}

VOID_NAMESPACE_CLOSE
