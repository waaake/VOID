// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <atomic>
#include <vector>
// #include <unordered_map>

/* Internal */
#include "Definition.h"
#include "FFmpegStream.h"

VOID_NAMESPACE_OPEN

class FFmpegDecoder2
{
public:
    FFmpegDecoder2();
    ~FFmpegDecoder2();

    FFmpegDecoder2(const FFmpegDecoder2&) = delete;
    FFmpegDecoder2(FFmpegDecoder2&&) = delete;
    FFmpegDecoder2& operator=(const FFmpegDecoder2&) = delete;
    FFmpegDecoder2& operator=(FFmpegDecoder2&&) = delete;

    static FFmpegDecoder2& Instance(const std::string& path);

    int Width() const { return m_VideoStream->Width(); }
    int Height() const { return m_VideoStream->Height(); }
    int Channels() const { return m_VideoStream->Channels(); }

    // bool Decode(const std::string& path, v_frame_t framenumber, Buffer<float>& pixels);

    bool Decode(const std::string& path, v_frame_t framenumber, std::vector<float>& pixels);
    bool Decode(const std::string& path, v_frame_t framenumber, std::vector<unsigned char>& pixels);

private:
    struct CacheEntry
    {
        std::vector<unsigned char> buffer;
        v_frame_t frame;
        CacheEntry() : frame(-1) {}
        CacheEntry(std::vector<unsigned char>& buffer, v_frame_t frame)
            : buffer(std::move(buffer)), frame(frame) {}
    };

    // std::unordered_map<v_frame_t, std::vector<unsigned char>> m_Cache;
    std::vector<CacheEntry> m_Cache;
    std::string m_Path;
    std::atomic<int> m_CacheIndex = 0;

    AVFormatContext* m_Format   = nullptr;
    AVPacket* m_Packet          = nullptr;
    FFmpegStream* m_VideoStream = nullptr;

private:
    bool Open(const std::string& path);
    void Close();
    int64_t DecodeAndStoreNext();
    void PullVideoBuffer(FFmpegStream* stream);

    // bool HasVideoCached(v_frame_t frame) const { return m_Cache.find(frame) != m_Cache.end(); }
    // std::vector<unsigned char> FreeFromVideoCache(v_frame_t framenumber);
    bool TryFromCache(v_frame_t framenumber, std::vector<unsigned char>& out);
    bool TryFromCache(v_frame_t framenumber, std::vector<float>& out);

    void FillBuffer(const std::vector<unsigned char>& in, std::vector<float>& out);
    inline static float Linear(float pixel) { return (pixel <= 0.04045f) ? pixel / 12.92f : powf((pixel + 0.055f) / 1.055f, 2.4f); }
};

VOID_NAMESPACE_CLOSE
