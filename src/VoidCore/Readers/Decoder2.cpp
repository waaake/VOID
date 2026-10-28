// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>
#include <cstring>
#include <memory>

/* TBB */
#include <tbb/parallel_for.h>

/* Internal */
#include "Decoder2.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

// This governs how many parallel instances of the Decoder can exist at ones
// allowing multiple streams of videos to be decoded for playback
// we may want to allow configuring this later on depending on system specs
// For now we allow 16 as something to test as well
static constexpr std::size_t MAX_DECODERS = 16;
static constexpr std::size_t MAX_CACHED_FRAMES = 6;

FFmpegDecoder2& FFmpegDecoder2::Instance(const std::string& path)
{
    static std::unordered_map<std::string, std::unique_ptr<FFmpegDecoder2>> s_decoders;
    static std::size_t s_head = 0;

    auto it = s_decoders.find(path);
    if (it == s_decoders.end())
    {
        std::unique_ptr<FFmpegDecoder2> decoder = nullptr;
        if (s_decoders.size() == MAX_DECODERS)
        {
            auto iter = s_decoders.begin();
            std::advance(iter, s_head);
            decoder = std::move(iter->second);
            s_decoders.erase(iter);
        }

        if (!decoder)
            decoder = std::make_unique<FFmpegDecoder2>();

        // Update the internal states and buffers if it's a different path
        decoder->Setup(path);
        s_decoders[path] = std::move(decoder);
        s_head = (s_head + 1) % MAX_DECODERS;
    }

    return *(s_decoders[path].get());
}

FFmpegDecoder2::FFmpegDecoder2()
{
    m_VideoCache.resize(MAX_CACHED_FRAMES);
}

FFmpegDecoder2::~FFmpegDecoder2()
{
    Close();
}

bool FFmpegDecoder2::Setup(const std::string& path)
{
    if (path != m_Path)
    {
        Close();
        if (!Open(path))
            return false;
    }

    return true;
}

bool FFmpegDecoder2::ComputeTimestamp(v_frame_t framenumber, TimeFrame& timeframe) const
{
    if (!m_VideoStream) return false;

    timeframe.timebase = m_VideoStream->StreamTimebase();
    timeframe.duration = av_rescale_q(1, av_inv_q(m_VideoStream->Framerate()), timeframe.timebase);
    timeframe.pts = framenumber * timeframe.duration;

    return true;
}

bool FFmpegDecoder2::FetchTimestamp(v_frame_t framenumber, TimeFrame& timeframe)
{
    if (TryCached(framenumber, timeframe))
        return true;

    /// If we want to decode backwards
    m_Backwards = (m_LastDecoded - framenumber) > 0;

    // When a previous frame than the current frame is requested, we want to seek backwars by a bit
    // this allows the decoder to try decoding all those frames to catch up to the requested number
    // when it does that, we will have the requested and previous n i.e. MAX_CACHED_FRAMES number of
    // frames cached already and that allows us to quickly go back to previous n frames without re-requesting so much
    if (m_Backwards)
        TrySeek(std::max(framenumber - (v_frame_t)MAX_CACHED_FRAMES, (v_frame_t)0), m_VideoStream);
    else if (framenumber - m_LastDecoded > MAX_CACHED_FRAMES) // else if the requested frame is too far ahead, we want to seek
        TrySeek(framenumber - 1, m_VideoStream);

    while (m_LastDecoded < framenumber)
    {
        int ret = DecodeAndStoreNext(m_VideoStream);
        if (ret == AVERROR_EOF)
            break;

        if (ret < 0)
            return false;
    }

    // Check now if we have frame
    if (TryCached(framenumber, timeframe, m_LastCacheIndex))
        return true;

    /// Could not decode the requested frame
    return false;
}

bool FFmpegDecoder2::Decode(v_frame_t framenumber, std::vector<float>& buffer, TimeFrame& timeframe)
{
    if (TryCached(framenumber, buffer, timeframe))
        return true;

    /// If we want to decode backwards
    m_Backwards = (m_LastDecoded - framenumber) > 0;

    // When a previous frame than the current frame is requested, we want to seek backwars by a bit
    // this allows the decoder to try decoding all those frames to catch up to the requested number
    // when it does that, we will have the requested and previous n i.e. MAX_CACHED_FRAMES number of
    // frames cached already and that allows us to quickly go back to previous n frames without re-requesting so much
    if (m_Backwards)
        TrySeek(std::max(framenumber - (v_frame_t)MAX_CACHED_FRAMES, (v_frame_t)0), m_VideoStream);
    else if (framenumber - m_LastDecoded > MAX_CACHED_FRAMES) // else if the requested frame is too far ahead, we want to seek
        TrySeek(framenumber - 1, m_VideoStream);
    else if (m_LastIndex != m_VideoStream->Index()) // If we were decoding audio before this, ensure we seek to somewhere before the requested frame
        TrySeek(framenumber, m_VideoStream);

    m_LastIndex = m_VideoStream->Index();
    while (m_LastDecoded < framenumber)
    {
        int ret = DecodeAndStoreNext(m_VideoStream);
        if (ret == AVERROR_EOF)
            break;

        if (ret < 0)
            return false;
    }

    // Check now if we have frame
    if (TryCached(framenumber, buffer, timeframe, m_LastCacheIndex))
        return true;

    /// Could not decode the requested frame
    return false;
}

bool FFmpegDecoder2::Decode(v_frame_t framenumber, std::vector<unsigned char>& buffer, TimeFrame& timeframe)
{
    if (TryCached(framenumber, buffer, timeframe))
        return true;

    while (m_LastDecoded < framenumber)
    {
        int ret = DecodeAndStoreNext(m_VideoStream);
        if (ret == AVERROR_EOF)
            break;

        if (ret < 0)
            return false;
    }

    // Check now if we have frame with a preferred index
    if (TryCached(framenumber, buffer, timeframe, m_LastCacheIndex))
        return true;

    /// Could not decode the requested frame
    return false;
}

bool FFmpegDecoder2::Decode(v_frame_t framenumber, const TimeFrame& timeframe, SharedFrameAudio& buffer)
{
    if (!m_AudioStream) return false;

    /// If we were decoding video before this, ensure we seek to somewhere before the requested frame
    if (m_LastIndex != m_AudioStream->Index())
        TrySeek(framenumber, m_AudioStream);

    m_LastIndex = m_AudioStream->Index();
    while (m_LastAudioPTS < FFmpegStream::ToAudioSamplePosition(timeframe.pts + timeframe.duration, timeframe.timebase))
    {
        int ret = DecodeAndStoreNext(m_AudioStream);
        if (ret == AVERROR_EOF)
            break;

        if (ret < 0)
            return false;
    }

    if (TryCached(framenumber, timeframe, buffer))
        return true;

    /// Could not decode the requested frame
    return false;
}

bool FFmpegDecoder2::Open(const std::string& path)
{
    if (avformat_open_input(&m_Format, path.c_str(), nullptr, nullptr) > 0)
        return false;

    if (avformat_find_stream_info(m_Format, nullptr) < 0)
        return false;

    int index = av_find_best_stream(m_Format, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (index < 0) return false;

    m_VideoStream = new FFmpegStream(m_Format, m_Format->streams[index], index, path);
    if (!m_VideoStream->Setup()) return false;

    index = av_find_best_stream(m_Format, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (index > -1) // Has audio
    {
        /// has audio, try to create and setup the audio stream and resampler
        /// if anything fails, then we reset the Audio stream and that makes the decoder behaves
        /// as if the container did not had any audio at all
        /// tbh, not so sure about returning false from here if we're not able to init audio stream
        m_AudioStream = new FFmpegStream(m_Format, m_Format->streams[index], index, path);
        if (!m_AudioStream->Setup())
        {
            delete m_AudioStream;
            m_AudioStream = nullptr;

            VOID_LOG_WARN("Unable to initialize Audio stream for the container. Check error log.");
        }
    }

    m_Packet = av_packet_alloc();

    m_Path = path;
    return true;
}

void FFmpegDecoder2::Close()
{
    if (m_VideoStream)
    {
        delete m_VideoStream;
        m_VideoStream = nullptr;
    }
    if (m_AudioStream)
    {
        delete m_AudioStream;
        m_AudioStream = nullptr;
    }

    if (m_Packet) av_packet_free(&m_Packet);
    if (m_Format) avformat_close_input(&m_Format);

    ResetCache();
}

void FFmpegDecoder2::ResetCache()
{
    for (DecodedFrame frame : m_VideoCache)
    {
        frame.pts = -1;
        frame.framenumber = -1;
    }
}

int64_t FFmpegDecoder2::DecodeAndStoreNext(FFmpegStream* stream)
{
    // Has some more frames that can be decoded from the last packet received
    if (stream->ReceiveFrame() == 0)
    {
        StoreBuffer(stream);
        return 0;
    }

    int ret = av_read_frame(m_Format, m_Packet);
    if (ret == AVERROR_EOF)
    {
        // We're at the end of file -- flush to get the delayed frames till ffmpeg can provide them
        stream->SendFlushPacket();
        while (stream->ReceiveFrame() == 0)
            StoreBuffer(stream);

        av_packet_unref(m_Packet);
        return AVERROR_EOF;
    }
    else if (ret == 0)
    {
        if (m_Packet->stream_index == stream->Index())
        {
            ret = stream->SendPacket(m_Packet);
            if (ret == 0 && stream->ReceiveFrame() == 0)
                StoreBuffer(stream);
        }
        else
        {
            av_packet_unref(m_Packet);
        }
    }
    else
    {
        av_packet_unref(m_Packet);
        AVLogError(ret);
    }

    return ret;
}

void FFmpegDecoder2::StoreBuffer(FFmpegStream* stream)
{
    if (stream->Type() == AVMEDIA_TYPE_VIDEO)
        StoreVideoBuffer(stream);
    else if (stream->Type() == AVMEDIA_TYPE_AUDIO)
        StoreAudioBuffer(stream);
}

void FFmpegDecoder2::StoreVideoBuffer(FFmpegStream* stream)
{
    DecodedFrame& frame = m_VideoCache[m_CacheIndex];
    if (stream->Decode(frame))
    {
        m_LastCacheIndex = m_CacheIndex.load();
        m_CacheIndex = (m_CacheIndex + 1) % MAX_CACHED_FRAMES;
        m_LastDecoded = frame.framenumber;
    }
}

void FFmpegDecoder2::StoreAudioBuffer(FFmpegStream* stream)
{
    AudioBuffer buffer;
    if (stream->Decode(buffer))
    {
        m_AudioBuffer.push_back(std::move(buffer));
        m_LastAudioPTS = stream->CurrentAudioSample();
    }
}

bool FFmpegDecoder2::TryCached(v_frame_t framenumber, TimeFrame& timeframe, int preferred)
{
    if (preferred > -1)
    {
        DecodedFrame& frame = m_VideoCache[preferred];
        if (frame.framenumber == framenumber)
        {
            timeframe.duration = frame.duration;
            timeframe.pts = frame.pts;
            timeframe.timebase = frame.timebase;
            return true;
        }
    }

    for (const DecodedFrame& entry : m_VideoCache)
    {
        if (entry.framenumber == framenumber)
        {
            timeframe.duration = entry.duration;
            timeframe.pts = entry.pts;
            timeframe.timebase = entry.timebase;
            return true;
        }
    }

    return false;
}

bool FFmpegDecoder2::TryCached(v_frame_t framenumber, std::vector<unsigned char>& out, TimeFrame& timeframe, int preferred)
{
    if (preferred > -1)
    {
        DecodedFrame& frame = m_VideoCache[preferred];
        if (frame.framenumber == framenumber)
        {
            out = frame.buffer;

            timeframe.duration = frame.duration;
            timeframe.pts = frame.pts;
            timeframe.timebase = frame.timebase;
            return true;
        }
    }

    for (const DecodedFrame& entry : m_VideoCache)
    {
        if (entry.framenumber == framenumber)
        {
            out = entry.buffer;

            timeframe.duration = entry.duration;
            timeframe.pts = entry.pts;
            timeframe.timebase = entry.timebase;
            return true;
        }
    }

    return false;
}

bool FFmpegDecoder2::TryCached(v_frame_t framenumber, std::vector<float>& out, TimeFrame& timeframe, int preferred)
{
    if (preferred > -1)
    {
        DecodedFrame& frame = m_VideoCache[preferred];
        if (frame.framenumber == framenumber)
        {
            FillBuffer(frame.buffer, out);
            timeframe.duration = frame.duration;
            timeframe.pts = frame.pts;
            timeframe.timebase = frame.timebase;
            return true;
        }
    }

    for (const DecodedFrame& entry : m_VideoCache)
    {
        if (entry.framenumber == framenumber)
        {
            FillBuffer(entry.buffer, out);
            timeframe.duration = entry.duration;
            timeframe.pts = entry.pts;
            timeframe.timebase = entry.timebase;
            return true;
        }
    }

    return false;
}

bool FFmpegDecoder2::TryCached(v_frame_t framenumber, const TimeFrame& timeframe, SharedFrameAudio& buffer)
{
    return ConvertAudio(framenumber, timeframe, buffer);
}

bool FFmpegDecoder2::ConvertAudio(v_frame_t framenumber, const TimeFrame& timeframe, SharedFrameAudio& buffer)
{
    const int64_t frameStart = FFmpegStream::ToAudioSamplePosition(timeframe.pts, timeframe.timebase);
    const int64_t frameEnd = FFmpegStream::ToAudioSamplePosition(timeframe.pts + timeframe.duration, timeframe.timebase);

    int index = 0;
    buffer->samplecount = frameEnd - frameStart;

    // Any missing audio in the buffer is set to 0 and remains silent
    buffer->samples.assign(static_cast<std::size_t>(buffer->samplecount * 2), 0.f);

    /// Fetch what index of the audio buffers we have that can provide the starting sample for the audio
    /// in the required video frame determined by the start pts
    while (index < static_cast<int>(m_AudioBuffer.size()))
    {
        const AudioBuffer& audio = m_AudioBuffer[index];
        const int64_t audioStart = audio.pts;
        const int64_t audioEnd = audio.pts + audio.sampleCount;

        if (audioEnd > frameStart)
            break;

        index++;
    }

    for (int i = index; i < static_cast<int>(m_AudioBuffer.size()); ++i)
    {
        const AudioBuffer& audio = m_AudioBuffer[i];

        const int64_t audioStart = audio.pts;
        const int64_t audioEnd = audio.pts + audio.sampleCount;

        if (audioStart >= frameEnd)
            break;

        const int64_t overlapStart = std::max(frameStart, audioStart);
        const int64_t overlapEnd = std::min(frameEnd, audioEnd);

        if (overlapStart >= overlapEnd)
            continue;

        const int64_t srcSample = overlapStart - audioStart;
        const int64_t dstSample = overlapStart - frameStart;
        const int64_t count = overlapEnd - overlapStart;

        const std::size_t srcIndex = static_cast<std::size_t>(srcSample) * audio.channels;
        const std::size_t dstIndex = static_cast<std::size_t>(dstSample) * audio.channels;

        const std::size_t valueCount = static_cast<std::size_t>(count) * audio.channels;

        buffer->channels = audio.channels;
        buffer->samplerate = audio.samplerate;

        if (srcIndex + valueCount > audio.samples.size() || dstIndex + valueCount > buffer->samples.size())
            return false;

        // VOID_LOG_INFO("Copying {} samples from source buffer index: {} to destination index: {} --- for frame: {}", valueCount, srcIndex, dstIndex, framenumber);
        audio.CopySamples(valueCount, srcIndex, buffer->samples.data() + dstIndex);
    }

    return true;
}

void FFmpegDecoder2::TrySeek(v_frame_t frame, FFmpegStream* stream)
{
    int64_t timestamp = stream->FrameToPts(frame);

    av_seek_frame(m_Format, stream->Index(), timestamp, AVSEEK_FLAG_BACKWARD);
    av_packet_unref(m_Packet);
    stream->FlushBuffers();

    m_LastDecoded = -1;
}

void FFmpegDecoder2::FillBuffer(const std::vector<unsigned char>& in, std::vector<float>& out)
{
    out.resize(in.size());

    float* buffer = out.data();
    int64_t count = static_cast<int64_t>(in.size());
    tbb::parallel_for(tbb::blocked_range<int64_t>(0, count), [&](const tbb::blocked_range<int64_t>& r) -> void
    {
        for (int64_t i = r.begin(); i != r.end(); ++i)
            buffer[i] = Linear(in[i] / 255.f);
    });
}

VOID_NAMESPACE_CLOSE
