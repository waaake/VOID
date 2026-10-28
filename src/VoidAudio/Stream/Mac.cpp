// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cstring>
#include <algorithm>
#include <thread>

/* Internal */
#include "Mac.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Timekeeper.h"

VOID_NAMESPACE_OPEN

AudioStream::AudioStream(int samplerate, int channels)
    : m_Samplerate(samplerate)
    , m_Channels(channels)
    , m_PlaybackTime(0.0)
    , m_SessionTime(0.0)
    , m_AudioUnit(nullptr)
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

        m_SessionTime = m_PlaybackTime;
    }
}

double AudioStream::Latency() const
{
    return 0.0;
}

bool AudioStream::WriteSamples(const unsigned char* buffer, std::size_t size, int samples)
{
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Samples.insert(m_Samples.end(), buffer, buffer + size);
    }

    // m_Cond.wait(lock, [&] { return m_Samples.empty(); });

    /** 
     * The Decoder expects this function to be blocking till the time the samples are played
     * well almost played to be precise, we don't want the buffer to underrun and cause stutters
     * Instead we wait just till the time the next samples can be played keeping enough time buffer
     * so that the additional processing for decoding and other bits can happen
     */
    std::this_thread::sleep_for(std::chrono::milliseconds((samples * 500 / m_Samplerate)));

    return true;
}

bool AudioStream::WriteSamples(const void* buffer, std::size_t size, int samples)
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    const float* fltbuf = static_cast<const float*>(buffer);
    m_FloatSamples.insert(m_FloatSamples.end(), fltbuf, fltbuf + size);
    return true;
}

void AudioStream::SeekTo(double time)
{
    m_SessionTime = time;
    if (m_AudioUnit)
    {
        AudioOutputUnitStop(m_AudioUnit);
        AudioUnitUninitialize(m_AudioUnit);

        // Safely clear any remaining samples from the buffer
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Samples.clear();
        }

        Start();
    }
}

OSStatus AudioStream::RenderCallback(void* inRefCon, AudioUnitRenderActionFlags* ioActionFlags, const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList* ioData)
{
    AudioStream* self = static_cast<AudioStream*>(inRefCon);
    AudioBuffer* output = &ioData->mBuffers[0];

    const std::size_t samplesRequested = static_cast<std::size_t>(inNumberFrames) * self->m_Channels;
    const std::size_t requestedBytes = samplesRequested * sizeof(float);

    std::memset(output->mData, 0, requestedBytes);

    {
        std::lock_guard<std::mutex> lock(self->m_Mutex);

        const std::size_t samplesToCopy = std::min(samplesRequested, self->m_FloatSamples.size());
        const std::size_t bytesToCopy = samplesToCopy * sizeof(float);
    
        if (bytesToCopy > 0)
        {
            std::memcpy(output->mData, self->m_FloatSamples.data(), bytesToCopy);
            self->m_FloatSamples.erase(self->m_FloatSamples.begin(), self->m_FloatSamples.begin() + samplesToCopy);
        }
    }

    return noErr;
}

VOID_NAMESPACE_CLOSE
