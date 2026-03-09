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
    m_Format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    m_Format.mBitsPerChannel = 16;
    m_Format.mChannelsPerFrame = channels;
    m_Format.mFramesPerPacket = 1;
    m_Format.mBytesPerFrame = (m_Format.mBitsPerChannel / 8) * channels;
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
        VOID_LOG_INFO("Played: {}s", m_SessionTime);
    }
}

double AudioStream::Latency() const
{
    return 0.0;
}

bool AudioStream::WriteSamples(const unsigned char* buffer, std::size_t size, int samples)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Samples.insert(m_Samples.end(), buffer, buffer + size);

    // m_Cond.wait(lock, [&] { return m_Samples.empty(); });

    return true;
}

OSStatus AudioStream::RenderCallback(void* inRefCon, AudioUnitRenderActionFlags* ioActionFlags, const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList* ioData)
{
    AudioStream* self = static_cast<AudioStream*>(inRefCon);

    VOID_LOG_INFO("inTimestamp: {0}, inBusNumber: {1}, inNumberFrames: {2}, seconds: {3}", inTimeStamp->mSampleTime, inBusNumber, inNumberFrames, inTimeStamp->mSampleTime / self->m_Samplerate);

    std::lock_guard<std::mutex> lock(self->m_Mutex);
    UInt32 bytesRequested = inNumberFrames * self->m_Channels * 2;
    UInt32 bytesAvailable = static_cast<UInt32>(self->m_Samples.size());

    UInt32 bytesToCopy = std::min(bytesRequested, bytesAvailable);

    VOID_LOG_INFO("Requested: {0}, Available: {1}, ToCopy: {2}", bytesRequested, bytesAvailable, bytesToCopy);

    if (bytesToCopy > 0)
    {
        std::memcpy(ioData->mBuffers[0].mData, self->m_Samples.data(), bytesToCopy);
        ioData->mBuffers[0].mDataByteSize = bytesToCopy;

        self->m_Samples.erase(self->m_Samples.begin(), self->m_Samples.begin() + bytesToCopy);
    }
    else
    {
        std::memset(ioData->mBuffers[0].mData, 0, bytesRequested);
        ioData->mBuffers[0].mDataByteSize = bytesRequested;
    }

    // if (self->m_Samples.size() < bytesRequested)
    // self->m_Cond.notify_all();

    self->m_PlaybackTime = inTimeStamp->mSampleTime / self->m_Samplerate;
    Timekeeper::Instance().SetTime(self->m_SessionTime + self->m_PlaybackTime);
    return noErr;
}

VOID_NAMESPACE_CLOSE
