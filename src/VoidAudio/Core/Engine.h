// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _AUDIO_PLAYER_H
#define _AUDIO_PLAYER_H

/* Internal */
#include "Definition.h"
#include "Audio.h"

VOID_NAMESPACE_OPEN

class AudioStream;

class VOID_API AudioEngine
{
public:
    AudioEngine();
    ~AudioEngine();

    bool Start();
    void Stop();

    void SetVolume(float volume);
    void Play(const SharedFrameAudio& buffer);

private:
    AudioStream* m_AudioStream;
};

VOID_NAMESPACE_CLOSE

#endif // _AUDIO_PLAYER_H
