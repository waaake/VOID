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

        s_decoders[path] = decoder == nullptr ? std::make_unique<FFmpegDecoder2>() : std::move(decoder);
        s_head = (s_head + 1) % MAX_DECODERS;
    }

    return *(s_decoders[path].get());
}

FFmpegDecoder2::FFmpegDecoder2()
{
    m_Cache.resize(MAX_CACHED_FRAMES);
}

FFmpegDecoder2::~FFmpegDecoder2()
{
    Close();
}

bool FFmpegDecoder2::Decode(const std::string& path, v_frame_t framenumber, std::vector<float>& buffer)
{
    if (path != m_Path)
    {
        Close();
        if (!Open(path))
            return false;
    }

    if (TryFromCache(framenumber, buffer))
        return true;

    while (m_VideoStream->CurrentFrame() < framenumber)
    {
        int ret = DecodeAndStoreNext();
        if (ret == AVERROR_EOF)
            break;
        
        if (ret < 0)
            return false;
    }

    // Check now if we have frame
    if (TryFromCache(framenumber, buffer))
        return true;

    /// Could not decode the requested frame
    return false;    
}

bool FFmpegDecoder2::Decode(const std::string& path, v_frame_t framenumber, std::vector<unsigned char>& buffer)
{
    if (path != m_Path)
    {
        Close();
        if (!Open(path))
            return false;
    }

    if (TryFromCache(framenumber, buffer))
        return true;

    while (m_VideoStream->CurrentFrame() < framenumber)
    {
        int ret = DecodeAndStoreNext();
        if (ret == AVERROR_EOF)
            break;
        
        if (ret < 0)
            return false;
    }

    // Check now if we have frame
    if (TryFromCache(framenumber, buffer))
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

    if (m_Packet) av_packet_free(&m_Packet);
    if (m_Format) avformat_close_input(&m_Format);
}

int64_t FFmpegDecoder2::DecodeAndStoreNext()
{
    // Has some more frames that can be decoded from the last packet received
    if (m_VideoStream->ReceiveFrame() == 0)
    {
        PullVideoBuffer(m_VideoStream);
        return 0;
    }

    int ret = av_read_frame(m_Format, m_Packet);
    if (ret == AVERROR_EOF)
    {
        // We're at the end of file -- flush to get the delayed frames till ffmpeg can provide them
        m_VideoStream->SendFlushPacket();
        while (m_VideoStream->ReceiveFrame() == 0)
            PullVideoBuffer(m_VideoStream);

        av_packet_unref(m_Packet);
        return AVERROR_EOF;
    }
    else if (ret == 0)
    {
        if (m_Packet->stream_index == m_VideoStream->Index())
        {
            ret = m_VideoStream->SendPacket(m_Packet);
            if (ret == 0 && m_VideoStream->ReceiveFrame() == 0)
                PullVideoBuffer(m_VideoStream);
        }
        else
        {
            av_packet_unref(m_Packet);
        }
    }
    else
    {
        av_packet_unref(m_Packet);
        VOID_LOG_INFO("Could not decode, errcode: {}", ret);
    }

    return ret;
}

void FFmpegDecoder2::PullVideoBuffer(FFmpegStream* stream)
{
    std::vector<unsigned char> buffer;
    if (stream->Decode(buffer))
    {
        VOID_LOG_INFO("Buffer size: {}", buffer.size());
        m_Cache.emplace(m_Cache.begin() + m_CacheIndex, buffer, stream->CurrentFrame());

        // m_Cache[stream->CurrentFrame()] = std::move(buffer);
        VOID_LOG_INFO("Pulled buffer for {}", stream->CurrentFrame());
        VOID_LOG_INFO("Buffer size ++ : {}", m_Cache[m_CacheIndex].buffer.size());

        m_CacheIndex = (m_CacheIndex + 1) % MAX_CACHED_FRAMES;
    }
}

// std::vector<unsigned char> FFmpegDecoder2::FreeFromVideoCache(v_frame_t frame)
// {
//     std::vector<unsigned char> buffer = m_Cache.at(frame);
//     // m_Cache.erase(frame);
//     return buffer;
// }

bool FFmpegDecoder2::TryFromCache(v_frame_t framenumber, std::vector<unsigned char>& out)
{
    for (const CacheEntry& entry : m_Cache)
    {
        if (entry.frame == framenumber)
        {
            // out = entry.buffer;
            out.resize(entry.buffer.size());
            std::memcpy(out.data(), entry.buffer.data(), entry.buffer.size());
            return true;
        }
    }

    return false;
}

bool FFmpegDecoder2::TryFromCache(v_frame_t framenumber, std::vector<float>& out)
{
    for (const CacheEntry& entry : m_Cache)
    {
        if (entry.frame == framenumber)
        {
            FillBuffer(entry.buffer, out);
            return true;
        }
    }

    return false;
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
