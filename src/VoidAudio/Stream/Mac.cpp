// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cstring>
#include <algorithm>
#include <thread>

/* Internal */
#include "Mac.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

AudioStream::AudioStream(int samplerate, int channels)
    : m_Samplerate(samplerate)
    , m_Channels(channels)
    , m_AudioUnit(nullptr)
    , m_Volume(0.1f)
{
    std::memset(&m_Format, 0, sizeof(m_Format));
    m_Format.mSampleRate = samplerate;
    m_Format.mFormatID = kAudioFormatLinearPCM;
    m_Format.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    m_Format.mBitsPerChannel = 32;
    m_Format.mChannelsPerFrame = channels;
    m_Format.mFramesPerPacket = 1;
    m_Format.mBytesPerFrame = sizeof(float) * channels;
    m_Format.mBytesPerPacket = m_Format.mBytesPerFrame;
}

AudioStream::~AudioStream()
{
    Stop();
    if (m_AudioUnit)
        AudioComponentInstanceDispose(m_AudioUnit);
    
    m_AudioUnit = nullptr;
}

bool AudioStream::Start()
{
    AudioComponentDescription desc;
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_DefaultOutput;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;

    AudioComponent component = AudioComponentFindNext(nullptr, &desc);
    if (!component)
        return false;

    if (AudioComponentInstanceNew(component, &m_AudioUnit) != noErr)
        return false;
    
    // Format
    if (AudioUnitSetProperty(m_AudioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &m_Format, sizeof(m_Format)) != noErr)
        return false;
    
    // RenderCallback
    AURenderCallbackStruct callback;
    callback.inputProc = RenderCallback;
    callback.inputProcRefCon = this;

    if (AudioUnitSetProperty(m_AudioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &callback, sizeof(callback)) != noErr)
        return false;
    
    if (AudioUnitInitialize(m_AudioUnit) != noErr)
        return false;
    
    if (AudioOutputUnitStart(m_AudioUnit) != noErr)
        return false;
    
    return true;
}

void AudioStream::Stop()
{
    if (m_AudioUnit)
    {
        AudioOutputUnitStop(m_AudioUnit);
        AudioUnitUninitialize(m_AudioUnit);
    }
}

bool AudioStream::WriteSamples(const void* buffer, std::size_t size, int samples)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::size_t offset = m_Samples.size();
    m_Samples.resize(m_Samples.size() + size);

    ApplyVolume(static_cast<const float*>(buffer), m_Samples.data() + offset, size);
    return true;
}

void AudioStream::Clear()
{
    // Clear anything that we currently have in unused buffer i.e. not yet queued on the device buffer
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Samples.clear();
    }
}

void AudioStream::SetVolume(float volume)
{
    m_Volume = std::clamp(volume, 0.f, 1.f);
}

double AudioStream::Latency() const
{
    return 0.0;
}

void AudioStream::ApplyVolume(const float* source, float* dest, std::size_t count)
{
    for (std::size_t i = 0; i < count; ++i)
        dest[i] = source[i] * m_Volume;
}

OSStatus AudioStream::RenderCallback(void* inRefCon, AudioUnitRenderActionFlags* ioActionFlags, const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList* ioData)
{
    AudioStream* self = static_cast<AudioStream*>(inRefCon);
    AudioBuffer* output = &ioData->mBuffers[0];

    const std::size_t samplesRequested = static_cast<std::size_t>(inNumberFrames) * self->m_Channels;
    std::memset(output->mData, 0, samplesRequested * sizeof(float));

    {
        std::lock_guard<std::mutex> lock(self->m_Mutex);
        const std::size_t samplesToCopy = std::min(samplesRequested, self->m_Samples.size());
        if (samplesToCopy > 0)
        {
            std::memcpy(output->mData, self->m_Samples.data(), samplesToCopy * sizeof(float));
            self->m_Samples.erase(self->m_Samples.begin(), self->m_Samples.begin() + samplesToCopy);
        }
    }

    return noErr;
}

VOID_NAMESPACE_CLOSE
