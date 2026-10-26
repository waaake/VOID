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

VOID_NAMESPACE_OPEN

AudioPlayer::AudioPlayer()
{
    m_AudioStream = new AudioStream(48000, 2, PA_SAMPLE_FLOAT32);
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
    m_AudioStream->WriteSamples(buffer->Data(), buffer->Bytes(), buffer->samplecount);
}

VOID_NAMESPACE_CLOSE
