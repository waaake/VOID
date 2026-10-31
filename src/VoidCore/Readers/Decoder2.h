// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <atomic>
#include <vector>

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

    bool Setup(const std::string& path);

    bool ComputeTimestamp(v_frame_t framenumber, TimeFrame& timeframe) const;
    bool FetchTimestamp(v_frame_t framenumber, TimeFrame& timeframe);
    bool Decode(v_frame_t framenumber, std::vector<float>& buffer, TimeFrame& timeframe);
    bool Decode(v_frame_t framenumber, std::vector<unsigned char>& buffer, TimeFrame& timeframe);
    bool Decode(v_frame_t framenumber, const TimeFrame& timeframe, SharedFrameAudio& buffer);

private:
    std::vector<DecodedFrame> m_VideoCache;
    std::vector<AudioBuffer> m_AudioBuffer;

    std::string m_Path;
    std::atomic<int> m_LastCacheIndex = 0;
    std::atomic<int> m_CacheIndex = 0;

    AVFormatContext* m_Format   = nullptr;
    AVPacket* m_Packet          = nullptr;
    FFmpegStream* m_VideoStream = nullptr;
    FFmpegStream* m_AudioStream = nullptr;

    int64_t m_LastDecoded       = -1;
    int64_t m_LastAudioPTS      = -1;
    bool m_Backwards            = false;
    int m_LastIndex             = -1;

private:
    bool Open(const std::string& path);
    void Close();
    void ResetCache();
    int64_t DecodeAndStoreNext(FFmpegStream* stream);
    void StoreBuffer(FFmpegStream* stream);
    void StoreVideoBuffer(FFmpegStream* stream);
    void StoreAudioBuffer(FFmpegStream* stream);

    bool TryCached(v_frame_t framenumber, TimeFrame& timeframe, int preferred = -1);
    bool TryCached(v_frame_t framenumber, std::vector<unsigned char>& out, TimeFrame& timeframe, int preferred = -1);
    bool TryCached(v_frame_t framenumber, std::vector<float>& out, TimeFrame& timeframe, int preferred = -1);
    bool TryCached(v_frame_t framenumber, const TimeFrame& timeframe, SharedFrameAudio& buffer);

    bool ConvertAudio(v_frame_t framenumber, const TimeFrame& timeframe, SharedFrameAudio& buffer);
    void TrySeek(v_frame_t framenumber, FFmpegStream* stream);

    void FillBuffer(const std::vector<unsigned char>& in, std::vector<float>& out);
    inline static float Linear(float pixel) { return (pixel <= 0.04045f) ? pixel / 12.92f : powf((pixel + 0.055f) / 1.055f, 2.4f); }
};

class AudioDecoder
{
public:
    AudioDecoder() = default;
    ~AudioDecoder();

    AudioDecoder(const AudioDecoder&) = delete;
    AudioDecoder& operator=(const AudioDecoder&) = delete;
    AudioDecoder(AudioBuffer&&) = delete;
    AudioDecoder& operator=(AudioDecoder&&) = delete;

    static AudioDecoder& Instance(const std::string& path);

    bool Setup(const std::string& path);
    bool ComputeTimestamp(v_frame_t framenumber, TimeFrame& timeframe) const;
    bool Decode(v_frame_t framenumber, const TimeFrame& timeframe, SharedFrameAudio& buffer);

private:
    std::vector<AudioBuffer> m_Buffer;
    std::string m_Path;

    AVFormatContext* m_Format   = nullptr;
    AVPacket* m_Packet          = nullptr;
    FFmpegStream* m_VideoStream = nullptr;
    FFmpegStream* m_AudioStream = nullptr;

    int64_t m_LastAudioPTS      = -1;
    bool m_Backwards            = false;

private:
    bool Open(const std::string& path);
    bool Failed();
    void Close();
    void ResetCache();

    int64_t DecodeAndStoreNext(FFmpegStream* stream);
    void StoreBuffer(FFmpegStream* stream);
    bool ConvertAudio(v_frame_t framenumber, const TimeFrame& timeframe, SharedFrameAudio& buffer);
};

VOID_NAMESPACE_CLOSE
