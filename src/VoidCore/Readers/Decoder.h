// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _FFMPEG_DECODER_H
#define _FFMPEG_DECODER_H

/* STD */
#include <mutex>
#include <vector>
#include <unordered_map>

/* FFMpeg */
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

/* Internal */
#include "Definition.h"
#include "PixReader.h"

VOID_NAMESPACE_OPEN

class FFmpegDecoder
{
public:
    static FFmpegDecoder& Instance(const std::string& path);

    FFmpegDecoder();
    ~FFmpegDecoder();    

    FFmpegDecoder(const FFmpegDecoder&) = delete;
    FFmpegDecoder(FFmpegDecoder&&) = delete;
    FFmpegDecoder& operator=(const FFmpegDecoder&) = delete;
    FFmpegDecoder& operator=(FFmpegDecoder&&) = delete;

    /**
     * Opens the movie file and decodes the frame
     * keeps the packet iterator in its state and does not destroy the context till a new filepath is
     * provided which means that subsequent queries to get consecutive/immediate frames would just be of
     * complexity O(1) generally as we're not iterating over the queried frame again
     * but sometimes the containers have frames not in order and that can lead to a next frame complexity of
     * O(n) as the iterator might have to loop over all frames to get to the one we're looking at which also
     * caches the other frames so next frame queries could directly result in direct data transfer
     */
    bool Decode(const std::string& path, const int framenumber, Buffer<float>& pixels);
    bool Decode(const std::string& path, const int framenumber, Buffer<unsigned char>& pixels);

    [[nodiscard]] int Width() const { return m_Width; }
    [[nodiscard]] int Height() const { return m_Height; }
    [[nodiscard]] int Channels() const { return m_Channels; }

private: /* Members */
    std::string m_Path;

    int64_t m_CurrentFrame;
    int m_Width, m_Height, m_Channels;

    /* FFMPEG Contexts */
    AVFormatContext* m_FormatContext;
    AVCodecContext* m_CodecContext;
    AVFrame* m_Frame;
    AVFrame* m_RGBFrame;
    AVPacket* m_Packet;
    SwsContext* m_SwsContext;
    AVStream* m_Stream;
    int m_StreamID;

    Buffer<unsigned char> m_Buffer;

    std::mutex m_Mutex;

private: /* Methods */
    void Open();
    void Close();

    /**
     * Decodes the next frame from the movie container
     * returns back the frame number (converted from av time base to signed long)
     */
    v_frame_t DecodeNextFrame();
    void FillBuffer(Buffer<float>& out);
    inline static float Linear(float pixel) { return (pixel <= 0.04045f) ? pixel / 12.92f : powf((pixel + 0.055f) / 1.055f, 2.4f); }
};

VOID_NAMESPACE_CLOSE

#endif // _FFMPEG_DECODER_H
